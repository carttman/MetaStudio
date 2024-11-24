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
#include "JYS/PlayerAnimInstance.h"
#include <MetaStudios/CHJ/MainGameInstance.h>
#include "Blueprint/UserWidget.h"
#include "Components/ArrowComponent.h"


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

	//FootstepAudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("FootstepAudioComp"));
	//FootstepAudioComp->SetupAttachment(RootComponent);
	//FootstepAudioComp->bAutoActivate = false;

	BoosterPackMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoosterPackMesh"));
	BoosterPackMesh->SetupAttachment(GetMesh(), TEXT("BoosterSocket"));
	BoosterPackMesh->SetVisibility(true);

	BoosterArrow1 = CreateDefaultSubobject<UArrowComponent>(TEXT("UArrowComponent"));
	BoosterArrow1->SetupAttachment(BoosterPackMesh);

	BoosterFXComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("BoosterFXComponent"));
	BoosterFXComponent->SetupAttachment(BoosterArrow1);

	BoosterFXComponent2 = CreateDefaultSubobject<UNiagaraComponent>(TEXT("BoosterFXComponent2"));
	BoosterFXComponent2->SetupAttachment(GetMesh());

	BoosterFXComponent3 = CreateDefaultSubobject<UNiagaraComponent>(TEXT("BoosterFXComponent3"));
	BoosterFXComponent3->SetupAttachment(GetMesh());
}

void AMetaStudiosCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority())
	{
		ManageBooster(DeltaTime);
	}
}

void AMetaStudiosCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
}

void AMetaStudiosCharacter::BeginPlay()
{
	Super::BeginPlay();

	////수정////////////////////////////////////
	TArray<AActor*> foundSpaceships;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpaceshipPawn::StaticClass(), foundSpaceships);

	if (foundSpaceships.Num() > 0)
	{
		for (int i = 0; i < foundSpaceships.Num(); ++i)
		{
			if (foundSpaceships[i] == nullptr)	continue;
			ListShip.Add(Cast<ASpaceshipPawn>(foundSpaceships[i]));
		}
		//spaceshipActor = Cast<ASpaceshipPawn>(foundSpaceships[0]);
	}

	TArray<AActor*> foundCars;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACarPawn::StaticClass(), foundCars);

	if (foundCars.Num() > 0)
	{
		for (int i = 0; i < foundCars.Num(); ++i)
		{
			if (foundCars[i] == nullptr)	continue;
			ListCar.Add(Cast<ACarPawn>(foundCars[i]));
		}
		//carActor = Cast<ACarPawn>(foundCars[0]);
	}


	// spaceshipActor = Cast<ASpaceshipPawn>(UGameplayStatics::GetActorOfClass(GetWorld(), SpaceshipPawnFactory));
	// carActor = Cast<ACarPawn>(UGameplayStatics::GetActorOfClass(GetWorld(), CarPawnFactory));

	ActivateBooster(false);

	Anim = Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance());

}

void AMetaStudiosCharacter::UsingBooster()
{
	ActivateBooster(bIsBoosting);
}

//////////////////////Animation/////////////////
void AMetaStudiosCharacter::Server_PlayAnimMontage_Implementation(class UAnimMontage* montageToPlay, float playRate /*= 1.0f*/, FName startSection /*= NAME_None*/)
{
	NetMulticast_PlayAnimMontage(montageToPlay, playRate, startSection);
}

void AMetaStudiosCharacter::NetMulticast_PlayAnimMontage_Implementation(class UAnimMontage* montageToPlay, float playRate /*= 1.0f*/, FName startSection /*= NAME_None*/)
{
	if (montageToPlay)
	{
		UE_LOG(LogTemp, Warning, TEXT("dddddddddddddd"))
	}
	if (Anim)
	{
		Anim->Montage_Play(montageToPlay, playRate, EMontagePlayReturnType::MontageLength);
		UE_LOG(LogTemp, Warning, TEXT("aaaaaaaaaaaaaaaaaaaaaaa"))
	}
	else
	{
		FString output;
		output = Anim == nullptr ? TEXT("NULL") : *Anim->GetName();
		UE_LOG(LogTemp, Warning, TEXT("%s"), *output);
	}

}

void AMetaStudiosCharacter::PickUpAnim()
{
	if (Anim)
	{
		Server_PlayAnimMontage(Anim->pickUpMontage);
	}
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
				// 매핑이 위로 쌓이기 떄문에 키가 겹쳐서 안될 수 있음 그래서 매핑콘테스트를 클리어해주고 AddMappingContext 해줘야함
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
		EnhancedInputComponent->BindAction(BoosterAction, ETriggerEvent::Started, this, &AMetaStudiosCharacter::ToggleBoosting);

		EnhancedInputComponent->BindAction(BoosterAction, ETriggerEvent::Completed, this, &AMetaStudiosCharacter::ToggleBoosting_Complete);

		// 물건 Destroy
		EnhancedInputComponent->BindAction(GetObjectAction, ETriggerEvent::Started, this, &AMetaStudiosCharacter::FindObject);

		// TPS/FPS 전환
		EnhancedInputComponent->BindAction(CameraMode, ETriggerEvent::Started, this, &AMetaStudiosCharacter::ChangeCameraMode);

		//// 우주선에서 앉았다가 일어나기 -> 컨트롤러 바뀌게////////
		PlayerInputComponent->BindAction("EnterSpaceship", IE_Pressed, this, &AMetaStudiosCharacter::SelectAutoMobile);
		//PlayerInputComponent->BindAction("EnterCar", IE_Pressed, this, &AMetaStudiosCharacter::EnterCar);


		EnhancedInputComponent->BindAction(IA_Exit, ETriggerEvent::Started, this, &AMetaStudiosCharacter::ExitSession);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component!"), *GetNameSafe(this));
	}
}

void AMetaStudiosCharacter::ResetEnhancedInputSetting(APlayerController* PlayerController)
{
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
				// 매핑이 위로 쌓이기 떄문에 키가 겹쳐서 안될 수 있음 그래서 매핑콘테스트를 클리어해주고 AddMappingContext 해줘야함
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

//void AMetaStudiosCharacter::AttachBoosterPack() const
//{
// 	const FName SocketName = TEXT("BoosterSocket");
//	if (!GetMesh()->DoesSocketExist(SocketName))
//	{
//		return;
//	}
//}

void AMetaStudiosCharacter::ManageBooster(float DeltaTime)
{
	//Server_ManageBooster(DeltaTime);
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
		ActivateBooster(bIsBoosting);
	}
	// 만약 부스터
	if (activeBooster && !bIsBoosting && !GetCharacterMovement()->IsFalling())
	{
		// 현재 부스터 + 리필 부스터 양(10) * DeltaTime
		BoosterAmount += BoosterRefillRate * DeltaTime;
		// 만약 현재 부스터가 최대 부스터보다 크다면
		if (BoosterAmount > MaxBoosterAmount)
		{
			// 현재 부스터를 최대 부스터로 초기화
			BoosterAmount = MaxBoosterAmount;
		}
		ActivateBooster(!bIsBoosting);
	}
}

void AMetaStudiosCharacter::Server_ManageBooster_Implementation(float DeltaTime)
{
	NetMulticast_ManageBooster_Implementation(DeltaTime);
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
		ActivateBooster(MoveStop);
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
	bIsBoosting = !bIsBoosting;
	GravityScaleOff();
}

void AMetaStudiosCharacter::NetMulticast_ToggleBoosting_Implementation()
{
	//// Only toggle if booster amount is not zero
	//if (BoosterAmount > 0)
	//{
	//	bIsBoosting = !bIsBoosting;
	//
	//	if (bIsBoosting)
	//	{
	//		GravityScaleOff();
	//	}
	//}
}

void AMetaStudiosCharacter::ToggleBoosting_Complete()
{
	Server_ToggleBoosting_Complete();
}

void AMetaStudiosCharacter::Server_ToggleBoosting_Complete_Implementation()
{
	bIsBoosting = false;
	ActivateBooster(bIsBoosting);
	GravityScaleOn();
	//NetMulticast_ToggleBoosting_Complete();
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

////////////////////////Effect///////////////////
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
	if (BoosterFXComponent2)
	{
		if (bActive)
		{
			BoosterFXComponent2->Activate();
		}
		else
		{
			BoosterFXComponent2->Deactivate();
		}
	}
}
/////////////////////Booster/////////////////////////////////////////////////////


/////////////카메라 전환 (TPS, FPS)////////////////나중에 녹화기능 할때 확인 필요
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

void AMetaStudiosCharacter::SelectAutoMobile()
{
	if (!IsLocallyControlled())	return;
	if (ListShip.Num() <= 0 && ListCar.Num() <= 0)	return;

	float distMinShip = 999999.0f;
	float distMinCar = 999999.0f;
	int selIdxShip = -1;
	int selIdxCar = -1;

	// Ship중 아무도 안타고 있고 가장 가까운 애
	for (int i = 0; i < ListShip.Num(); ++i)
	{
		float distCur = GetDistanceTo(ListShip[i]);
		if (distMinShip < distCur)	continue;
		if( ListShip[i]->bExistRider == true )	continue;
		distMinShip = distCur;
		selIdxShip = i;
	}

	// Car중 아무도 안타고 있고 가장 가까운 애
	for (int i = 0; i < ListCar.Num(); ++i)
	{
		float distCur = GetDistanceTo(ListCar[i]);
		if (distMinCar < distCur)	continue;
		if (ListCar[i]->bExistRider == true)	continue;
		distMinCar = distCur;
		selIdxCar = i;
	}

	if (selIdxShip == -1 && selIdxCar == -1)	return;

	if (distMinShip < distMinCar)
	{
		spaceshipActor = ListShip[selIdxShip];
		GetController()->SetControlRotation(spaceshipActor->GetActorRotation());
		Server_EnterSpaceship(spaceshipActor);
	}
	else
	{
		carActor = ListCar[selIdxCar];
		GetController()->SetControlRotation(carActor->GetActorRotation());
		Server_EnterCar(carActor);
	}

	/*
	if (spaceshipActor != nullptr && carActor != nullptr)
	{
		float spaceshipDist = GetDistanceTo(spaceshipActor);
		float carDist = GetDistanceTo(carActor);

		if (carDist < spaceshipDist)
		{
			GetController()->SetControlRotation(carActor->GetActorRotation());
			Server_EnterCar();
		}
		else
		{
			GetController()->SetControlRotation(spaceshipActor->GetActorRotation());
			Server_EnterSpaceship();
		}
	}
	else
	{
		//if (spaceshipActor != nullptr)
		//	Server_EnterSpaceship();
		//else if (carActor != nullptr)
		//	Server_EnterCar();
	}
	*/
}

//////////////////// 우주선이랑 플레이어랑 컨트롤러 바꾸기 ///////////////////
//void AMetaStudiosCharacter::EnterSpaceship()
//{
//	float spaceshipDist = GetDistanceTo(spaceshipActor);
//	float carDist = GetDistanceTo(carActor);
//
//	if (carDist < spaceshipDist)
//		return;
//
//
//	if (IsLocallyControlled())
//	{
//		Server_EnterSpaceship();
//	}
//
//	// Possess가 된 후 widget 사라지기
//	if (spaceshipActor->ActiveWidget)
//	{
//		spaceshipActor->ActiveWidget = CreateWidget<UUserWidget>(GetWorld(), spaceshipActor->WidgetClass);
//		spaceshipActor->ActiveWidget->RemoveFromViewport();
//	}
//}

void AMetaStudiosCharacter::Server_EnterSpaceship_Implementation(class AActor* TargetActor)
{
	if (HasAuthority())
	{
		if (TargetActor == nullptr)	return;

		ASpaceshipPawn* SpaceshipActor = Cast<ASpaceshipPawn>(TargetActor);
		if (SpaceshipActor == nullptr)	return;
		if (SpaceshipActor->bExistRider == true)
		{
			UE_LOG(LogTemp, Warning, TEXT("Spaceship is already possessed by another player!"));
			return;
		}

		if (SpaceshipActor->CanPlayerEnter(this))
		{
			SpaceshipActor->player = this;
			SpaceshipActor->bExistRider = true;
			GetController()->SetControlRotation(SpaceshipActor->GetActorRotation());
			GetController()->Possess(SpaceshipActor);
			NetMulticast_EnterSpaceship(SpaceshipActor);
		}

		//ASpaceshipPawn* SpaceshipActor = Cast<ASpaceshipPawn>(UGameplayStatics::GetActorOfClass(GetWorld(), SpaceshipPawnFactory));
		//if (SpaceshipActor == nullptr)
		//{
		//	FActorSpawnParameters SpawnParams;
		//	SpaceshipActor = GetWorld()->SpawnActor<ASpaceshipPawn>(SpaceshipPawnFactory, SpawnParams);
		//	UE_LOG(LogTemp, Error, TEXT("Failed to find or spawn SpaceshipActor."));
		//}
		//
		//if (SpaceshipActor == nullptr)	return;
		//
		//if (SpaceshipActor->player != nullptr)
		//{
		//	UE_LOG(LogTemp, Warning, TEXT("Spaceship is already possessed by another player!"));
		//	return;
		//}
		//
		//if (SpaceshipActor->CanPlayerEnter(this))
		//{
		//	SpaceshipActor->player = this;
		//	GetController()->SetControlRotation(SpaceshipActor->GetActorRotation());
		//	GetController()->Possess(SpaceshipActor);
		//	NetMulticast_EnterSpaceship(SpaceshipActor);
		//}
	}
}

void AMetaStudiosCharacter::NetMulticast_EnterSpaceship_Implementation(ASpaceshipPawn* SpaceshipActor)
{
	if (SpaceshipActor)
	{
		SpaceshipActor->player = this;
		SpaceshipActor->bExistRider = true;
		GetMesh()->SetVisibility(false);

		if (IsLocallyControlled())
		{
			SpaceshipActor->ResetEnhancedInputSetting(Cast<APlayerController>(GetWorld()->GetFirstPlayerController()));
			// Possess가 된 후 widget 사라지기
			if (SpaceshipActor->ActiveWidget != nullptr)
			{
				SpaceshipActor->ActiveWidget->RemoveFromParent();
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Spaceship class could not be loaded!"));
	}
}

/// 자동차랑 플레이어랑 컨트롤러 바꾸기
//void AMetaStudiosCharacter::EnterCar()
//{
//
//	float spaceshipDist = GetDistanceTo(spaceshipActor);
//	float carDist = GetDistanceTo(carActor);
//
//	if (carDist >= spaceshipDist)
//		return;
//
//	if (IsLocallyControlled())
//	{
//		Server_EnterCar();
//	}
//
//}

void AMetaStudiosCharacter::Server_EnterCar_Implementation(class AActor* TargetActor)
{
	if (HasAuthority())
	{
		if (TargetActor == nullptr)	return;

		ACarPawn* CarActor = Cast<ACarPawn>(TargetActor);
		if (CarActor == nullptr)	return;
		if (CarActor->bExistRider == true)
		{
			UE_LOG(LogTemp, Warning, TEXT("Car is already possessed by another player!"));
			return;
		}

		if (CarActor->CanPlayerEnterCar(this))
		{
			CarActor->player = this;
			CarActor->bExistRider = true;
			GetController()->SetControlRotation(CarActor->GetActorRotation());
			GetController()->Possess(CarActor);
			NetMulticast_EnterCar(CarActor);
		}


		//ACarPawn* CarActor = Cast<ACarPawn>(UGameplayStatics::GetActorOfClass(GetWorld(), CarPawnFactory));
		//if (CarActor == nullptr)
		//{
		//	FActorSpawnParameters SpawnParams;
		//	CarActor = GetWorld()->SpawnActor<ACarPawn>(CarPawnFactory, SpawnParams);
		//	UE_LOG(LogTemp, Error, TEXT("Failed to find or spawn SpaceshipActor."));
		//}
		//
		//if (CarActor == nullptr)	return;
		//
		///// 한명만 Possess 되어 있게
		//if (CarActor->player != nullptr)
		//{
		//	UE_LOG(LogTemp, Warning, TEXT("Car is already possessed by another player!"));
		//	return;
		//}
		//
		//if (CarActor->CanPlayerEnterCar(this))
		//{
		//	CarActor->player = this;
		//	GetController()->SetControlRotation(CarActor->GetActorRotation());
		//	GetController()->Possess(CarActor);
		//
		//	// Possess가 된 후 widget 사라지기
		//	spaceshipActor->ActiveWidget = CreateWidget<UUserWidget>(GetWorld(), spaceshipActor->WidgetClass);
		//	spaceshipActor->ActiveWidget->RemoveFromViewport();
		//
		//
		//	NetMulticast_EnterCar(CarActor);
		//}
	}
}

void AMetaStudiosCharacter::NetMulticast_EnterCar_Implementation(ACarPawn* CarActor)
{
	if (CarActor)
	{
		CarActor->player = this;
		CarActor->bExistRider = true;
		GetMesh()->SetVisibility(false);
		CarActor->RidingPlayer->SetVisibility(true);

		if (IsLocallyControlled())
		{
			CarActor->ResetEnhancedInputSetting(Cast<APlayerController>(GetWorld()->GetFirstPlayerController()));
			// Possess가 된 후 widget 사라지기
			if (CarActor->ActiveWidget != nullptr)
			{
				CarActor->ActiveWidget->RemoveFromParent();
			}
		}

		//CarActor->SetActorRotation(CarActor->GetActorRotation());
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

		//if (!MovementVector.IsZero())
		//{
		//	if (!FootstepAudioComp->IsPlaying())
		//	{
		//		bAutoActivate = true;
		//		FootstepAudioComp->Play();
		//	}
		//}
		//else 
		//{
		//	if (FootstepAudioComp->IsPlaying())
		//	{
		//		bAutoActivate= false;
		//		FootstepAudioComp->Stop();
		//	}
		//}
	}
}

void AMetaStudiosCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		if (!Controller->GetPawn()->IsA(ASpaceshipPawn::StaticClass()))
		{
			AddControllerYawInput(-LookAxisVector.X);
			AddControllerPitchInput(LookAxisVector.Y);
		}
	}
}

//////// 가까운 물건 Destroy /////////
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
		PickUpAnim();
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

void AMetaStudiosCharacter::ExitSession()
{
	auto gi = Cast<UMainGameInstance>(GetWorld()->GetGameInstance());
	UE_LOG(LogTemp, Warning, TEXT("exit"));

	if (gi)
	{
		UE_LOG(LogTemp, Warning, TEXT("exit"));
		gi->ExitSession();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("non_exit"));

	}
}

void AMetaStudiosCharacter::AddVehicle(class AActor* target)
{
	if( target == nullptr )	return;
	
	ASpaceshipPawn* SpaceShip = Cast<ASpaceshipPawn>(target);
	if (SpaceShip != nullptr)
	{
		ListShip.Add(SpaceShip);
		return;
	}
	
	ACarPawn* Car = Cast<ACarPawn>(target);
	if (Car != nullptr)
	{
		ListCar.Add(Car);
		return;
	}	
}

void AMetaStudiosCharacter::DeleteVehicle(AActor* target)
{
	if( ListShip.Remove(Cast<ASpaceshipPawn>(target)) >= 0 )	return;
	ListCar.Remove(Cast<ACarPawn>(target));
}

//void AMetaStudiosCharacter::AddVehicle(class ASpaceshipPawn* target)
//{
//	if (target == nullptr)	return;
//	ListShip.Add(Cast<ASpaceshipPawn>(target));
//}
//
//void AMetaStudiosCharacter::AddVehicle(class ACarPawn* target)
//{
//	if (target == nullptr)	return;
//	ListCar.Add(Cast<ACarPawn>(target));
//}
//
//void AMetaStudiosCharacter::DeleteVehicle(class ASpaceshipPawn* target)
//{
//	if (target == nullptr)	return;
//	ListShip.Remove(target);
//}
//
//void AMetaStudiosCharacter::DeleteVehicle(class ACarPawn* target)
//{
//	if (target == nullptr)	return;
//	ListCar.Remove(target);
//}

