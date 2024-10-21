#include "JYS/MetaStudiosCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "EngineMinimal.h"
#include "Net/UnrealNetwork.h"
#include "kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "GameFramework/Controller.h"
#include "JYS/SpaceshipPawn.h"
#include "JYS/CharacterController.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

////////////////////////////////////////////////////////////////////////////
// AMetaStudiosCharacter
AMetaStudiosCharacter::AMetaStudiosCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	/////////////카메라 전환 (TPS, FPS)////////////////
	// Create a camera boom
	TPSCameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("TPSCameraSpringArm"));
	TPSCameraSpringArm->SetupAttachment(RootComponent);
	TPSCameraSpringArm->TargetArmLength = 400.0f;
	TPSCameraSpringArm->bUsePawnControlRotation = true;
	TPSCameraSpringArm->TargetArmLength = 200;

	// Create a follow camera
	TPSCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("TPSCamera"));
	TPSCamera->SetupAttachment(TPSCameraSpringArm);
	TPSCamera->bUsePawnControlRotation = false;

	// Create a camera boom
	FPSCameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("FPSCameraSpringArm"));
	FPSCameraSpringArm->SetupAttachment(RootComponent);
	FPSCameraSpringArm->TargetArmLength = 0.0f;
	FPSCameraSpringArm->bUsePawnControlRotation = true;
	FPSCameraSpringArm->TargetArmLength = 0;

	// Create a follow camera
	FPSCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FPSCamera"));
	FPSCamera->SetupAttachment(FPSCameraSpringArm);
	FPSCamera->bUsePawnControlRotation = false;



}

void AMetaStudiosCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	ManageBooster(DeltaTime);
}

void AMetaStudiosCharacter::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* playerController = Cast<APlayerController>(GetController());
	//playerController->SetViewTargetWithBlend(TPSCamera->GetChildActor(), 2.f)

	if (FPSCamera == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("111111111111"))
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Blue, TEXT("2222222222222222"));

	}

}
//////////////////////Input////////////////////////////////
void AMetaStudiosCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMetaStudiosCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMetaStudiosCharacter::Look);

		// Booster Toggle
		EnhancedInputComponent->BindAction(BoosterAction, ETriggerEvent::Triggered, this, &AMetaStudiosCharacter::ToggleBoosting);

		EnhancedInputComponent->BindAction(BoosterAction, ETriggerEvent::Completed, this, &AMetaStudiosCharacter::ToggleBoosting_Complete);

		EnhancedInputComponent->BindAction(GetObjectAction, ETriggerEvent::Started, this, &AMetaStudiosCharacter::FindObject);

		EnhancedInputComponent->BindAction(CameraMode, ETriggerEvent::Started, this, &AMetaStudiosCharacter::ChangeCameraMode);

		//// 우주선에서 앉았다가 일어나기 -> 컨트롤러 바뀌게////////
		PlayerInputComponent->BindAction("EnterSpaceship", IE_Pressed, this, &AMetaStudiosCharacter::EnterSpaceship);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component!"), *GetNameSafe(this));
	}
}

//////////////////// 우주선이랑 플레이어랑 컨트롤러 바꾸기 ///////////////////
void AMetaStudiosCharacter::EnterSpaceship()
{
	APlayerController* characterController = Cast<APlayerController>(GetWorld()->GetFirstPlayerController());

	ASpaceshipPawn* SpaceshipActor = Cast<ASpaceshipPawn>(UGameplayStatics::GetActorOfClass(GetWorld(), SpaceshipPawnFactory));

	if (SpaceshipActor)
	{
		if (characterController && SpaceshipActor)
		{
			SpaceshipActor->player = this;
			if (SpaceshipActor->CanPlayerEnter())
			{
				characterController->Possess(SpaceshipActor);
				GetMesh()->SetVisibility(false);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Spaceship class could not be loaded!"));
	}
}

/////////////////////Booster/////////////////////////////////////////////////////

void AMetaStudiosCharacter::ManageBooster(float DeltaTime)
{
	Server_ManageBooster(DeltaTime);
}

void AMetaStudiosCharacter::Server_ManageBooster_Implementation(float DeltaTime)
{
	NetMulticast_ManageBooster(DeltaTime);
}

void AMetaStudiosCharacter::NetMulticast_ManageBooster_Implementation(float DeltaTime)
{
	// bIsBoosting 활성화 + 현재 남은 부스터가 0 이상일때
	if (bIsBoosting && BoosterAmount > 0.0f)
	{
		// 현재 부스터 - 20 * DeltaTime
		BoosterAmount -= BoosterDrainRate * DeltaTime;


		// 만약 현재 부스터 남은 양이 0 이하거나 같을 때
		if (BoosterAmount <= 0.0f)
		{
			// 부스터 남은 양을 0으로 초기화
			BoosterAmount = 0.0f;
			bIsBoosting = false;
			GravityScaleOff();
		}

		FVector HoverForce = FVector(GetVelocity().X, GetVelocity().Y, BoostStrength * DeltaTime);
		LaunchCharacter(HoverForce, true, true);

	}
	// 만약 부스터
	if (!bIsBoosting && !GetCharacterMovement()->IsFalling())
	{
		// 현재 부스터 + 리필 부스터 양(10) * DeltaTime
		BoosterAmount += BoosterRefillRate * DeltaTime;
		// 만약 현재 부스터가 최대 부스터보다 크다면
		if (BoosterAmount > MaxBoosterAmount)
		{
			// 현재 부스터를 최대 부스터로 초기화
			BoosterAmount = MaxBoosterAmount;
		}
	}

	// GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Blue, FString::Printf(TEXT("Booster Amount: %.1f"), BoosterAmount));

}

void AMetaStudiosCharacter::ToggleBoosting()
{
	Server_ToggleBoosting();
}

void AMetaStudiosCharacter::Server_ToggleBoosting_Implementation()
{
	NetMulticast_ToggleBoosting();
}

void AMetaStudiosCharacter::NetMulticast_ToggleBoosting_Implementation()
{
	// Only toggle if booster amount is not zero
	if (BoosterAmount > 0)
	{
		bIsBoosting = !bIsBoosting;

		if (bIsBoosting)
		{
			GravityScaleOff();
		}
	}
}

void AMetaStudiosCharacter::ToggleBoosting_Complete()
{
	Server_ToggleBoosting_Complete();
}

void AMetaStudiosCharacter::Server_ToggleBoosting_Complete_Implementation()
{
	NetMulticast_ToggleBoosting_Complete();
}

void AMetaStudiosCharacter::NetMulticast_ToggleBoosting_Complete_Implementation()
{
	bIsBoosting = false;
	GravityScaleOn();
}

void AMetaStudiosCharacter::GravityScaleOff()
{
	// 중력 끄기
	GetCharacterMovement()->GravityScale = GravityScaleZero;

}

void AMetaStudiosCharacter::GravityScaleOn()
{
	// 중력 켜기
	GetCharacterMovement()->GravityScale = GravityScaleNormal;
}

/////////////////////Booster/////////////////////////////////////////////////////


/////////////카메라 전환 (TPS, FPS)////////////////
void AMetaStudiosCharacter::ChangeCameraMode()
{
	if (IsTPSMode)
	{
		TPSCamera->SetActive(true);
		FPSCamera->SetActive(false);
		bUseControllerRotationYaw = true;
	}
	else
	{
		if (FPSCamera)
		{
			FPSCamera->SetActive(true);
			TPSCamera->SetActive(false);
			bUseControllerRotationYaw = false;
		}
	}

	IsTPSMode = !IsTPSMode;
}

void AMetaStudiosCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMetaStudiosCharacter, bIsBoosting);
}

void AMetaStudiosCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AMetaStudiosCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AMetaStudiosCharacter::FindObject()
{
	//AActor* findObject = UGameplayStatics::GetActorOfClass(GetWorld(), object);

	auto playerLoc = GetWorld()->GetFirstPlayerController()->GetCharacter()->GetActorLocation();

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), object, objects);

	nearActor = UGameplayStatics::FindNearestActor(playerLoc, objects, nearsetObjectDist);

	AActor* nearestObject = nullptr;

	if (nearActor != nullptr)
	{
		DestroyObject();
	}
}

void AMetaStudiosCharacter::DestroyObject()
{
	if (nearActor != nullptr)
	{
		nearActor->Destroy();
		// itemCount++;
	}

}
