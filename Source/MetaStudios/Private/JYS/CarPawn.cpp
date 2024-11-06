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
	//ThrusterComponent->Deactivate();	
	ThrusterComponent2 = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ThrusterComponent2"));
	ThrusterComponent2->SetupAttachment(CarMesh);
	//ThrusterComponent2->Deactivate();



}

// Called when the game starts or when spawned
void ACarPawn::BeginPlay()
{
	Super::BeginPlay();

	//APlayerController* playerController = Cast<APlayerController>(GetController());
	ActivateThruster(false);	
}

// Called every frame
void ACarPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	/*
	FTransform trans = FTransform(this->GetControlRotation());
	direction = trans.TransformVector(direction);
	direction.Z = 0;
	direction.Normalize();

	AddMovementInput(direction);


	direction = FVector::ZeroVector;
	*/

	if (MoveStop == true)
	{
		ApplyRollBack();
		ActivateThruster(false);
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
	if( v.X <= 0.0f )
	{			
		MoveStop = true;
		return;
	}
	
	MoveStop = false;
	direction.X = v.X;
	//direction.Y = v.Y;
	//direction.Y = 0.0f;
	direction.Normalize();	
	
	if (direction.X != 0.0f)
	{
		ApplyRoll(v.Y);
	}	
	else
	{
		ApplyRollBack();
	}

	//// 이동하고 싶다.
	//	// 회전 방향으로 이동하고싶다.
	//	// 1. ControlRotation 을 이용해서 Transform을 만들고
	FTransform ttt = FTransform(GetControlRotation());
	direction = ttt.TransformVector(direction);
	direction.Z = 0;
	direction.Normalize();
	////// 2. TransformDirection기능 이용해서 방향을 만들어서
	////// 3. 그 방향으로 이동
	AddMovementInput(direction);
	direction = FVector::ZeroVector;

	ActivateThruster(true);
	
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
				// 매핑이 위로 쌓이기 떄문에 키가 겹쳐서 안될 수 있음 그래서 매핑콘테스트를 클리어해주고 AddMappingContext 해줘야함
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
	//CurrentRotation.Yaw = FMath::FInterpTo(CurrentRotation.Yaw, YawAmount, GetWorld()->GetDeltaSeconds(), 2.0f);
	CurrentRotation.Roll = FMath::FInterpTo(CurrentRotation.Roll, RollAmount, GetWorld()->GetDeltaSeconds(), 3.0f);
	// P = P0 + vt

	float YawCur = CurrentRotation.Yaw;
	
	CurrentRotation.Yaw = YawCur + 50.0f * RollInput * GetWorld()->DeltaTimeSeconds;

	
	// Apply the modified rotation back to the mesh
	//CarMesh->SetRelativeRotation(CurrentRotation);
	//SetActorRotation(CurrentRotation);
	GetController()->SetControlRotation(CurrentRotation);




}

void ACarPawn::ApplyRollBack()
{
	UE_LOG(LogTemp, Warning, TEXT("ApplyRollBack"));
	// Get the current rotation of the mesh
	FRotator CurrentRotation = GetControlRotation();// CarMesh->GetRelativeRotation();

	// Calculate the roll amount
	float RollAmount = CurrentRotation.Roll;
	float YawAmount = CurrentRotation.Yaw;

	if( RollAmount == 0.0f && YawAmount == 0.0f )	return;

	UE_LOG(LogTemp, Warning, TEXT("ApplyRollBack R :%f : Y : %f"), RollAmount, YawAmount);
	// Interpolate towards the new roll value	
	CurrentRotation.Yaw = FMath::FInterpTo(CurrentRotation.Yaw, 0.0f, GetWorld()->GetDeltaSeconds(), 2.0f);
	CurrentRotation.Roll = FMath::FInterpTo(CurrentRotation.Roll, 0.0f, GetWorld()->GetDeltaSeconds(), 3.0f);
	// P = P0 + vt

	//float YawCur = CurrentRotation.Yaw;

	//CurrentRotation.Yaw = YawCur + 50.0f * RollInput * GetWorld()->DeltaTimeSeconds;


	// Apply the modified rotation back to the mesh
	//CarMesh->SetRelativeRotation(CurrentRotation);
	//SetActorRotation(CurrentRotation);
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



