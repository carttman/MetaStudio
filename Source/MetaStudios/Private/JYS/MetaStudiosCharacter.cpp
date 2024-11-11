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
#include "JYS/CarPawn.h"
#include "../../../../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraComponent.h"

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

	/////////////ī�޶� ��ȯ (TPS, FPS)////////////////
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

	BoosterFXComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("BoosterFXComponent"));
	BoosterFXComponent->SetupAttachment(GetMesh());


}

void AMetaStudiosCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	ManageBooster(DeltaTime);

}

void AMetaStudiosCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

}

void AMetaStudiosCharacter::BeginPlay() 
{
	Super::BeginPlay();

	spaceshipActor = Cast<ASpaceshipPawn>(UGameplayStatics::GetActorOfClass(GetWorld(), SpaceshipPawnFactory ));
	carActor = Cast<ACarPawn>(UGameplayStatics::GetActorOfClass(GetWorld(), CarPawnFactory));

	ActivateBooster(false);

}

//////////////////////Input////////////////////////////////
void AMetaStudiosCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//UE_LOG(LogTemp, Error, TEXT("Player SetupPlayerInputComponent"));
	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			if (IsLocallyControlled())
			{
				// ������ ���� ���̱� ������ Ű�� ���ļ� �ȵ� �� ���� �׷��� �������׽�Ʈ�� Ŭ�������ְ� AddMappingContext �������
				Subsystem->ClearAllMappings();
				Subsystem->AddMappingContext(DefaultMappingContext, 0);

			}
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

		// ���� Destroy
		EnhancedInputComponent->BindAction(GetObjectAction, ETriggerEvent::Started, this, &AMetaStudiosCharacter::FindObject);

		// TPS/FPS ��ȯ
		EnhancedInputComponent->BindAction(CameraMode, ETriggerEvent::Started, this, &AMetaStudiosCharacter::ChangeCameraMode);

		//// ���ּ����� �ɾҴٰ� �Ͼ�� -> ��Ʈ�ѷ� �ٲ��////////
		PlayerInputComponent->BindAction("EnterSpaceship", IE_Pressed, this, &AMetaStudiosCharacter::EnterSpaceship);
		PlayerInputComponent->BindAction("EnterCar", IE_Pressed, this, &AMetaStudiosCharacter::EnterCar);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component!"), *GetNameSafe(this));
	}
}

void AMetaStudiosCharacter::ResetEnhancedInputSetting(APlayerController* PlayerController)
{
	UE_LOG(LogTemp, Error, TEXT("ResetEnhancedInputSetting"));

	if (PlayerController)
	{
		if (PlayerController->GetLocalPlayer() == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("Player SetupPlayerInputComponent111111111111111111111111111111111111111111"));
		}

		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetWorld()->GetFirstPlayerController()->GetLocalPlayer()))
		{
			if (IsLocallyControlled())
			{
				// ������ ���� ���̱� ������ Ű�� ���ļ� �ȵ� �� ���� �׷��� �������׽�Ʈ�� Ŭ�������ְ� AddMappingContext �������
				Subsystem->ClearAllMappings();
				Subsystem->AddMappingContext(DefaultMappingContext, 0);

			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Player SetupPlayerInputComponent3333333333333333333333333333"));
		}
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
	// bIsBoosting Ȱ��ȭ + ���� ���� �ν��Ͱ� 0 �̻��϶�
	if (bIsBoosting && BoosterAmount > 0.0f)
	{
		// ���� �ν��� - 20 * DeltaTime
		BoosterAmount -= BoosterDrainRate * DeltaTime;


		// ���� ���� �ν��� ���� ���� 0 ���ϰų� ���� ��
		if (BoosterAmount <= 0.0f)
		{
			// �ν��� ���� ���� 0���� �ʱ�ȭ
			BoosterAmount = 0.0f;
			bIsBoosting = false;
			GravityScaleOff();
		}

		FVector HoverForce = FVector(GetVelocity().X, GetVelocity().Y, BoostStrength * DeltaTime);
		LaunchCharacter(HoverForce, true, true);
		ActivateBooster(MoveStop);
	}
	// ���� �ν���
	if (!bIsBoosting && !GetCharacterMovement()->IsFalling())
	{
		// ���� �ν��� + ���� �ν��� ��(10) * DeltaTime
		BoosterAmount += BoosterRefillRate * DeltaTime;
		// ���� ���� �ν��Ͱ� �ִ� �ν��ͺ��� ũ�ٸ�
		if (BoosterAmount > MaxBoosterAmount)
		{
			// ���� �ν��͸� �ִ� �ν��ͷ� �ʱ�ȭ
			BoosterAmount = MaxBoosterAmount;
		}
		ActivateBooster(!MoveStop);
	}

	// GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Blue, FString::Printf(TEXT("Booster Amount: %.1f"), BoosterAmount));

}

void AMetaStudiosCharacter::ToggleBoosting()
{
	Server_ToggleBoosting(true);
}

void AMetaStudiosCharacter::Server_ToggleBoosting_Implementation(bool bMove)
{
	NetMulticast_ToggleBoosting();
	MoveStop = bMove;
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
	// �߷� ����
	GetCharacterMovement()->GravityScale = GravityScaleZero;

}

void AMetaStudiosCharacter::GravityScaleOn()
{
	// �߷� �ѱ�
	GetCharacterMovement()->GravityScale = GravityScaleNormal;
}

void AMetaStudiosCharacter::ActivateBooster(bool bActive)
{
	if (activeBooster == bActive)	return;
	activeBooster = bActive;

	if (BoosterFXComponent)
	{
		if (bActive)
		{
			BoosterFXComponent->Activate();
		}
		else
		{
			BoosterFXComponent->Deactivate();
		}
	}
}
/////////////////////Booster/////////////////////////////////////////////////////


/////////////ī�޶� ��ȯ (TPS, FPS)////////////////���߿� ��ȭ��� �Ҷ� Ȯ�� �ʿ�
void AMetaStudiosCharacter::ChangeCameraMode()
{
	Server_ChangeCameraMode();
}

void AMetaStudiosCharacter::Server_ChangeCameraMode_Implementation()
{
	NetMulticast_ChangeCameraMode();
}

void AMetaStudiosCharacter::NetMulticast_ChangeCameraMode_Implementation()
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

//////////////////// ���ּ��̶� �÷��̾�� ��Ʈ�ѷ� �ٲٱ� ///////////////////
void AMetaStudiosCharacter::EnterSpaceship()
{
	float spaceshipDist = GetDistanceTo(spaceshipActor);
	float carDist = GetDistanceTo(carActor);

	if (carDist < spaceshipDist)
		return;

	if (IsLocallyControlled())
	{
		Server_EnterSpaceship();
	}
}

void AMetaStudiosCharacter::Server_EnterSpaceship_Implementation()
{
	if (HasAuthority())
	{
		ASpaceshipPawn* SpaceshipActor = Cast<ASpaceshipPawn>(UGameplayStatics::GetActorOfClass(GetWorld(), SpaceshipPawnFactory));
		if (SpaceshipActor == nullptr)
		{
			FActorSpawnParameters SpawnParams;
			SpaceshipActor = GetWorld()->SpawnActor<ASpaceshipPawn>(SpaceshipPawnFactory, SpawnParams);
			UE_LOG(LogTemp, Error, TEXT("Failed to find or spawn SpaceshipActor."));
		}
		else
		{
			SpaceshipActor->player = this;
			if (SpaceshipActor->CanPlayerEnter(this))
			{
				GetController()->Possess(SpaceshipActor);
				UE_LOG(LogTemp, Error, TEXT("Change Possess to spawn SpaceshipActor."));
			}
		}
		NetMulticast_EnterSpaceship(SpaceshipActor);
	}
}

void AMetaStudiosCharacter::NetMulticast_EnterSpaceship_Implementation(ASpaceshipPawn* SpaceshipActor)
{
	if (SpaceshipActor)
	{
		SpaceshipActor->player = this;
		GetMesh()->SetVisibility(false);
		SpaceshipActor->ResetEnhancedInputSetting(Cast<APlayerController>(GetWorld()->GetFirstPlayerController()));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Spaceship class could not be loaded!"));
	}
}

/// �ڵ����� �÷��̾�� ��Ʈ�ѷ� �ٲٱ�
void AMetaStudiosCharacter::EnterCar()
{

	float spaceshipDist = GetDistanceTo(spaceshipActor);
	float carDist = GetDistanceTo(carActor);

	if( carDist >= spaceshipDist )
	return;

	if (IsLocallyControlled())
	{
		Server_EnterCar();
	}
}

void AMetaStudiosCharacter::Server_EnterCar_Implementation()
{
	if (HasAuthority())
	{
		ACarPawn* CarActor = Cast<ACarPawn>(UGameplayStatics::GetActorOfClass(GetWorld(), CarPawnFactory));
		if (CarActor == nullptr)
		{
			FActorSpawnParameters SpawnParams;
			CarActor = GetWorld()->SpawnActor<ACarPawn>(CarPawnFactory, SpawnParams);
			UE_LOG(LogTemp, Error, TEXT("Failed to find or spawn SpaceshipActor."));
		}
		else
		{
			CarActor->player = this;
			if (!this && CarActor->CanPlayerEnterCar(this))
			{
				GetController()->Possess(CarActor);
				UE_LOG(LogTemp, Error, TEXT("Change Possess to spawn SpaceshipActor."));
			}
		}
		NetMulticast_EnterCar(CarActor);
	}

}

void AMetaStudiosCharacter::NetMulticast_EnterCar_Implementation(ACarPawn* CarActor)
{
	if (CarActor)
	{
		CarActor->player = this;
		GetMesh()->SetVisibility(false);
		CarActor->ResetEnhancedInputSetting(Cast<APlayerController>(GetWorld()->GetFirstPlayerController()));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Spaceship class could not be loaded!"));
	}
}

void AMetaStudiosCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMetaStudiosCharacter, bIsBoosting);
	DOREPLIFETIME(AMetaStudiosCharacter, IsTPSMode);

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
		AddControllerYawInput(-LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

//////// ����� ���� Destroy /////////
void AMetaStudiosCharacter::FindObject()
{
	Server_FindObject();
}

void AMetaStudiosCharacter::Server_FindObject_Implementation()
{
	NetMulticast_FindObject();
}

void AMetaStudiosCharacter::NetMulticast_FindObject_Implementation()
{
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
