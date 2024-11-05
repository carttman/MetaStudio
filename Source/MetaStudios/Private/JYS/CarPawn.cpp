// Fill out your copyright notice in the Description page of Project Settings.


#include "JYS/CarPawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputSubsystems.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputComponent.h"
#include "../../../../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraComponent.h"
#include "../../../../Plugins/FX/Niagara/Source/Niagara/Classes/NiagaraSystem.h"
#include "../../../../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraFunctionLibrary.h"



// Sets default values
ACarPawn::ACarPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DefaultScene = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneComp"));
	SetRootComponent(DefaultScene);

	CarMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CarMesh"));
	CarMesh->SetupAttachment(DefaultScene);

	
	//SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	//SpringArm->SetupAttachment(CarMesh);
	//SpringArm->TargetArmLength = 300.0f;
	//
	//CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	//CameraComp->SetupAttachment(RootComponent);

	ThrusterComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ThrusterComponent"));
	ThrusterComponent->SetupAttachment(CarMesh);
	ThrusterComponent->Deactivate();	
	ThrusterComponent2 = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ThrusterComponent2"));
	ThrusterComponent2->SetupAttachment(CarMesh);
	ThrusterComponent2->Deactivate();


	////////////����Ʈ �߰�/////////////

	//static ConstructorHelpers::FObjectFinder<UNiagaraSystem> ThrusterEffectAsset(TEXT("/Game/RocketThrusterExhaustFX/FX/NS_RocketExhaust_Afterburn_Jet.NS_RocketExhaust_Afterburn_Jet"));
	//if (ThrusterEffectAsset.Succeeded())
	//{
	//	ThrusterFXSystem = ThrusterEffectAsset.Object;
	//}
}

// Called when the game starts or when spawned
void ACarPawn::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* playerController = Cast<APlayerController>(GetController());

}

// Called every frame
void ACarPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FTransform trans = FTransform(this->GetControlRotation());
	direction = trans.TransformVector(direction);
	direction.Z = 0;
	direction.Normalize();

	AddMovementInput(direction);


	direction = FVector::ZeroVector;
	Server_UpdateTransform(GetActorLocation(), GetActorRotation());
}

// Called to bind functionality to input
void ACarPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

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
	
	PlayerInputComponent->BindAction("ExitCar", IE_Pressed, this, &ACarPawn::ExitCar);


	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(CarLook, ETriggerEvent::Triggered, this, &ACarPawn::OnMyActionLook);
		EnhancedInputComponent->BindAction(CarMove, ETriggerEvent::Triggered, this, &ACarPawn::OnMyActionMove);
	}

}

bool ACarPawn::CanPlayerEnterCar(AMetaStudiosCharacter* targetCharacter)
{
	if (targetCharacter == nullptr)	return false;

	float distance = GetDistanceTo(targetCharacter);
	return distance <= 1000.0f;
}

void ACarPawn::ExitCar()
{
	if (IsLocallyControlled())
	{
		Server_ExitCar();
	}

}

void ACarPawn::OnMyActionMove(const FInputActionValue& value)
{
	FVector2D v = value.Get<FVector2D>();
	direction.X = v.X;
	//direction.Y = v.Y;
	direction.Y = 0.0f;
	direction.Normalize();

	ApplyRoll(v.Y);

	// �̵��ϰ� �ʹ�.
		// ȸ�� �������� �̵��ϰ�ʹ�.
		// 1. ControlRotation �� �̿��ؼ� Transform�� �����
	FTransform ttt = FTransform(GetControlRotation());
	direction = ttt.TransformVector(direction);
	direction.Z = 0;
	direction.Normalize();
	//// 2. TransformDirection��� �̿��ؼ� ������ ����
	//// 3. �� �������� �̵�
	AddMovementInput(direction);
	direction = FVector::ZeroVector;


	//ThrusterComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ThrusterFXSystem, GetActorLocation());

	if (ThrusterComponent)
	{
		ThrusterComponent->Activate();
	}
	if (ThrusterComponent2)
	{
		ThrusterComponent2->Activate();
	}

	
}

void ACarPawn::OnMyActionLook(const FInputActionValue& value)
{
	FVector2D v = value.Get<FVector2D>();
	//AddControllerPitchInput(-v.Y * 0.5f);
	//AddControllerRollInput(v.X * 0.5f);
	if(v.X != 0.0f )
	{
		//ApplyRoll(v.X * 0.5f);
	}
}

void ACarPawn::ResetEnhancedInputSetting(APlayerController* pc)
{
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

}

void ACarPawn::ApplyRoll(float RollInput)
{
	// Get the current rotation of the mesh
	FRotator CurrentRotation =  GetControlRotation();// CarMesh->GetRelativeRotation();

	// Calculate the roll amount
	float RollAmount = RollInput * 45.0f; // Adjust the multiplier for sensitivity
	float YawAmount = RollInput * 45.0f;	

	// Interpolate towards the new roll value	
	CurrentRotation.Yaw = FMath::FInterpTo(CurrentRotation.Yaw, YawAmount, GetWorld()->GetDeltaSeconds(), 2.0f);
	CurrentRotation.Roll = FMath::FInterpTo(CurrentRotation.Roll, RollAmount, GetWorld()->GetDeltaSeconds(), 3.0f);

	
	// Apply the modified rotation back to the mesh
	//CarMesh->SetRelativeRotation(CurrentRotation);
	//SetActorRotation(CurrentRotation);
	GetController()->SetControlRotation(CurrentRotation);
}

void ACarPawn::Server_UpdateTransform_Implementation(FVector newLocation, FRotator newRotation)
{
	SetActorLocationAndRotation(newLocation, newRotation);
}

void ACarPawn::Server_ExitCar_Implementation()
{
	if (HasAuthority())
	{
		//APlayerController* characterController = Cast<APlayerController>(GetWorld()->GetFirstPlayerController());
		if (player)
		{
			FVector carLoc = GetActorLocation();
			FRotator carRot = GetActorRotation();

			FVector offset = carRot.RotateVector(FVector(200.0f, 100.0f, 0.0f));
			FVector playerSpawnLocation = carLoc + offset;

			player->SetActorLocation(playerSpawnLocation);
			player->SetActorRelativeRotation(carRot);

			GetController()->Possess(player);
			UE_LOG(LogTemp, Error, TEXT("Change Possess to spawn Player"));
			//player->GetMesh()->SetVisibility(true);
		}

	}
	NetMulticast_ExitCar();

}

void ACarPawn::NetMulticast_ExitCar_Implementation()
{
	if (player)
	{
		FVector carLoc = GetActorLocation();
		FRotator carRot = GetActorRotation();

		FVector offset = carRot.RotateVector(FVector(200.0f, 0.0f, 0.0f));
		FVector playerSpawnLocation = carLoc + offset;

		player->SetActorLocation(playerSpawnLocation);
		player->SetActorRelativeRotation(carRot);

		//characterController->Possess(player);
		player->GetMesh()->SetVisibility(true);

		player->ResetEnhancedInputSetting(Cast<APlayerController>(GetWorld()->GetFirstPlayerController()));
	}

}



