// Fill out your copyright notice in the Description page of Project Settings.


#include "JYS/CarPawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputSubsystems.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputComponent.h"
#include "../../../../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraComponent.h"
#include "../../../../Plugins/FX/Niagara/Source/Niagara/Classes/NiagaraSystem.h"
#include "../../../../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraFunctionLibrary.h"
#include "Net/UnrealNetwork.h"



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
	ThrusterComponent2 = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ThrusterComponent2"));
	ThrusterComponent2->SetupAttachment(CarMesh);

	RidingPlayer = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("playerVisible"));
	RidingPlayer->SetupAttachment(CarMesh);

}

// Called when the game starts or when spawned
void ACarPawn::BeginPlay()
{
	Super::BeginPlay();

	ActivateThruster(false);	
}

// Called every frame
void ACarPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ActivateThruster(!MoveStop);
	if (MoveStop == true)
	{
		ApplyRollBack();		
	}

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
				// 매핑이 위로 쌓이기 떄문에 키가 겹쳐서 안될 수 있음 그래서 매핑콘테스트를 클리어해주고 AddMappingContext 해줘야함
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
		EnhancedInputComponent->BindAction(CarMove, ETriggerEvent::Completed, this, &ACarPawn::OnMyActionMove);
	}

}

void ACarPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACarPawn, MoveStop);
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
	if( v.X <= 0.0f && MoveStop == false)
	{			
		//MoveStop = true;
		Server_OnMyActionMove(true);
		return;
	}
	
	//MoveStop = false;
	direction.X = v.X;
	direction.Normalize();	
	
	if (direction.X != 0.0f)
	{
		ApplyRoll(v.Y);
	}	
	else
	{
		ApplyRollBack();
	}

	FTransform ttt = FTransform(GetControlRotation());
	direction = ttt.TransformVector(direction);
	direction.Z = 0;
	direction.Normalize();
	AddMovementInput(direction);
	direction = FVector::ZeroVector;
	
	//ActivateThruster(true);
	Server_OnMyActionMove(false);
}

void ACarPawn::Server_OnMyActionMove_Implementation(bool bMove)
{
	MoveStop = bMove;
}

void ACarPawn::OnMyActionLook(const FInputActionValue& value)
{
	FVector2D v = value.Get<FVector2D>();
	//if(v.X != 0.0f )
	//{
	//	//ApplyRoll(v.X * 0.5f);
	//}
}

void ACarPawn::ResetEnhancedInputSetting(APlayerController* pc)
{
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

}

void ACarPawn::ApplyRoll(float RollInput)
{
	FRotator CurrentRotation =  GetControlRotation();

	float RollAmount = RollInput * 45.0f; 
	float YawAmount = RollInput * 45.0f;	

	CurrentRotation.Roll = FMath::FInterpTo(CurrentRotation.Roll, RollAmount, GetWorld()->GetDeltaSeconds(), 3.0f);

	float YawCur = CurrentRotation.Yaw;
	
	// P = P0 + vt
	CurrentRotation.Yaw = YawCur + 50.0f * RollInput * GetWorld()->DeltaTimeSeconds;

	GetController()->SetControlRotation(CurrentRotation);
}

void ACarPawn::ApplyRollBack()
{
	FRotator CurrentRotation = GetControlRotation();

	float RollAmount = CurrentRotation.Roll;
	//float YawAmount = CurrentRotation.Yaw;

	if( RollAmount == 0.0f /*&& YawAmount == 0.0f*/ )	return;

	//CurrentRotation.Yaw = FMath::FInterpTo(CurrentRotation.Yaw, 0.0f, GetWorld()->GetDeltaSeconds(), 2.0f);
	CurrentRotation.Roll = FMath::FInterpTo(CurrentRotation.Roll, 0.0f, GetWorld()->GetDeltaSeconds(), 3.0f);

	GetController()->SetControlRotation(CurrentRotation);
}

void ACarPawn::ActivateThruster(bool bActive)
{
	if(ActiveThruster == bActive )	return;
	ActiveThruster = bActive;

	if (ThrusterComponent )
	{
		if (bActive)
		{
			ThrusterComponent->Activate();
		}
		else
		{
			ThrusterComponent->Deactivate();
		}		
	}

	if (ThrusterComponent2)
	{
		if (bActive)
		{
			ThrusterComponent2->Activate();
		}
		else
		{
			ThrusterComponent2->Deactivate();
		}
	}
}

void ACarPawn::Server_UpdateTransform_Implementation(FVector newLocation, FRotator newRotation)
{
	SetActorLocationAndRotation(newLocation, newRotation);
}

void ACarPawn::Server_ExitCar_Implementation()
{
	if (HasAuthority())
	{
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
		RidingPlayer->SetVisibility(false);

		player->ResetEnhancedInputSetting(Cast<APlayerController>(GetWorld()->GetFirstPlayerController()));
	}

}



