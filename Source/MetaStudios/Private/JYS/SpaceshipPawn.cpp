// Fill out your copyright notice in the Description page of Project Settings.


#include "JYS/SpaceshipPawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Animation/AnimInstance.h"
#include "../../../../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraComponent.h"
#include "JYS/SpaceshipAnimInstance.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ASpaceshipPawn::ASpaceshipPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpaceshipMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SpaceshipMesh"));
	RootComponent = SpaceshipMesh;

	SpaceshipSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SpaceshipSkeletalMesh"));
	SpaceshipSkeletalMesh->SetupAttachment(SpaceshipMesh);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 1000.0f;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArm);

	/*startFlyFXComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("startFlyEffectComponent"));
	startFlyFXComponent->SetupAttachment(SpaceshipSkeletalMesh);*/
}

// Called when the game starts or when spawned
void ASpaceshipPawn::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* playerController = Cast<APlayerController>(GetController());
	Anim = Cast<USpaceshipAnimInstance>(SpaceshipSkeletalMesh->GetAnimInstance());
	if (Anim)
	{
		Anim->SetLegUpMontagePlayRate();
	}
	// ActivateStartFly(false);
}


// Called every frame 
void ASpaceshipPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!IsLocallyControlled())	return;
	FTransform trans = FTransform(this->GetControlRotation());
	direction = trans.TransformVector(direction);
	// direction.Z = 0;
	direction.Normalize();

	AddMovementInput(direction);
	direction = FVector::ZeroVector;
	Server_UpdateTransform(GetActorLocation(), GetActorRotation());

	FRotator currentRotation = GetActorRotation();
	currentRotation.Pitch = 0.0f;
	SetActorRotation(currentRotation);

	// ActivateStartFly(!MoveStop);
}

// Called to bind functionality to input
void ASpaceshipPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	UE_LOG(LogTemp, Error, TEXT("Spaceship SetupPlayerInputComponent"));
	PlayerInputComponent->BindAction("ExitSpaceship", IE_Pressed, this, &ASpaceshipPawn::ExitSpaceship);

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

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(SpaceshipLook, ETriggerEvent::Triggered, this, &ASpaceshipPawn::OnMyActionLook);
		EnhancedInputComponent->BindAction(SpaceshipMove, ETriggerEvent::Triggered, this, &ASpaceshipPawn::OnMyActionMove);
		EnhancedInputComponent->BindAction(SpaceshipMoveUp, ETriggerEvent::Triggered, this, &ASpaceshipPawn::OnMoveUp);
		EnhancedInputComponent->BindAction(SpaceshipMoveDown, ETriggerEvent::Triggered, this, &ASpaceshipPawn::OnMoveDown);
	}
}

void ASpaceshipPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{

	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}

bool ASpaceshipPawn::CanPlayerEnter(AMetaStudiosCharacter* targetCharacter)
{
	// 플레이어와 우주선 사이의 간격

	if (targetCharacter == nullptr)	return false;

	float distance = GetDistanceTo(targetCharacter);
	return distance <= 1000.0f;
}

void ASpaceshipPawn::ExitSpaceship()
{
	if (IsLocallyControlled())
	{
		Server_ExitSpaceship();
	}
}

void ASpaceshipPawn::Server_ExitSpaceship_Implementation()
{
	if (HasAuthority())
	{
		//APlayerController* characterController = Cast<APlayerController>(GetWorld()->GetFirstPlayerController());
		if (player)
		{
			FVector spaceshipLoc = GetActorLocation();
			FRotator spaceshipRot = GetActorRotation();

			FVector offset = spaceshipRot.RotateVector(FVector(200.0f, 0.0f, 0.0f));
			FVector playerSpawnLocation = spaceshipLoc + offset;

			player->SetActorLocation(playerSpawnLocation);
			player->SetActorRelativeRotation(spaceshipRot);

			GetController()->Possess(player);
			UE_LOG(LogTemp, Error, TEXT("Change Possess to spawn Player"));
			//player->GetMesh()->SetVisibility(true);
		}

	}
	NetMulticast_ExitSpaceship();
}

void ASpaceshipPawn::NetMulticast_ExitSpaceship_Implementation()
{
	//APlayerController* characterController = Cast<APlayerController>(GetWorld()->GetFirstPlayerController());

	if (player)
	{
		FVector spaceshipLoc = GetActorLocation();
		FRotator spaceshipRot = GetActorRotation();

		FVector offset = spaceshipRot.RotateVector(FVector(200.0f, 300.0f, 0.0f));
		FVector playerSpawnLocation = spaceshipLoc + offset;

		player->SetActorLocation(playerSpawnLocation);
		player->SetActorRelativeRotation(spaceshipRot);

		//characterController->Possess(player);
		player->GetMesh()->SetVisibility(true);

		player->ResetEnhancedInputSetting(Cast<APlayerController>(GetWorld()->GetFirstPlayerController()));
	}
}

void ASpaceshipPawn::ResetEnhancedInputSetting(class APlayerController* pc)
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

//////////////////////이동//////////////////////////////
void ASpaceshipPawn::OnMyActionMove(const FInputActionValue& value)
{
	//FVector2D v = value.Get<FVector2D>();
	//direction.X = v.X;
	//direction.Y = v.Y;
	//direction.Z = v.Y;
	//direction.Normalize();

	FVector2D v = value.Get<FVector2D>();
	if (v.X <= 0.0f && MoveStop == false)
	{
		//MoveStop = true;
		Server_OnMyActionMoveSpaceship(true);
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

	Server_OnMyActionMoveSpaceship(false);
}


void ASpaceshipPawn::Server_OnMyActionMoveSpaceship_Implementation(bool bMove)
{
	MoveStop = bMove;
}

void ASpaceshipPawn::OnMyActionLook(const FInputActionValue& value)
{
	FVector2D v = value.Get<FVector2D>();

	AddControllerPitchInput(-v.Y);
	AddControllerYawInput(v.X);
}

void ASpaceshipPawn::OnMoveUp(const FInputActionValue& value)
{
	if (bCantMove)	return;
	UE_LOG(LogTemp, Warning, TEXT("OnMoveUP"))
		FVector currentLocation = GetActorLocation();
	// 수직 하강
	currentLocation.Z += MovementSpeed * GetWorld()->GetDeltaSeconds();
	SetActorLocation(currentLocation);

	if (bLanded)
	{
		bLanded = false;
		MoveUpAnim();
	}

	StartFlyEffect();
	SetActorLocation(currentLocation);
}

void ASpaceshipPawn::OnMoveDown(const FInputActionValue& value)
{
	if (bCantMove)	return;
	// 이미 착지중이거나 지면에 닿았으면 움직이지 않고 return;
	if (bLanded || CheckLanding()) return;

	FVector currentLocation = GetActorLocation();
	// 수직 하강
	currentLocation.Z -= MovementSpeed * GetWorld()->GetDeltaSeconds();
	SetActorLocation(currentLocation);


}

void ASpaceshipPawn::ApplyRoll(float RollInput)
{
	FRotator CurrentRotation = GetControlRotation();

	float RollAmount = RollInput * 45.0f;
	float YawAmount = RollInput * 45.0f;

	CurrentRotation.Roll = FMath::FInterpTo(CurrentRotation.Roll, RollAmount, GetWorld()->GetDeltaSeconds(), 3.0f);

	float YawCur = CurrentRotation.Yaw;

	// P = P0 + vt
	CurrentRotation.Yaw = YawCur + 50.0f * RollInput * GetWorld()->DeltaTimeSeconds;

	GetController()->SetControlRotation(CurrentRotation);

}

void ASpaceshipPawn::ApplyRollBack()
{
	FRotator CurrentRotation = GetControlRotation();

	float RollAmount = CurrentRotation.Roll;
	//float YawAmount = CurrentRotation.Yaw;

	if (RollAmount == 0.0f /*&& YawAmount == 0.0f*/)	return;

	//CurrentRotation.Yaw = FMath::FInterpTo(CurrentRotation.Yaw, 0.0f, GetWorld()->GetDeltaSeconds(), 2.0f);
	CurrentRotation.Roll = FMath::FInterpTo(CurrentRotation.Roll, 0.0f, GetWorld()->GetDeltaSeconds(), 3.0f);

	GetController()->SetControlRotation(CurrentRotation);

}

void ASpaceshipPawn::Server_SpaceshipUpdateTransform_Implementation(FVector newLocation, FRotator newRotation)
{
	SetActorLocationAndRotation(newLocation, newRotation);
}

//////////////////이동//////////////////////////////

void ASpaceshipPawn::StartFlyEffect()
{
	FVector start = GetActorLocation();
	FVector end = start - FVector(0, 0, 1000.0f);

	FHitResult hitResult;
	FCollisionQueryParams params;
	params.AddIgnoredActor(this);
	if (GetWorld()->LineTraceSingleByChannel(hitResult, start, end, ECC_Visibility, params))
	{
		DrawDebugLine(GetWorld(), start, end, FColor::Green, false, 1.0f, 0, 5.0f);
		float targetZ = hitResult.Location.Z;
		FVector currentLocation = GetActorLocation();

		float distanceToGround = FVector::Dist(currentLocation, hitResult.Location);

		if (distanceToGround > 0.0f)
		{
			currentLocation.Z += 500.0f * GetWorld()->GetDeltaSeconds();

			/////////////////우주선 출발할 때 이펙트/////////////////
			//ActivateStartFly(true);
		}

		if (FVector::Dist(currentLocation, hitResult.Location) > 10.0f)
		{
			currentLocation.Z = targetZ;
		}
		SetActorLocation(currentLocation);
	}

}



//void ASpaceshipPawn::ActivateStartFly(bool bActive)
//{
//	if (activeStartFly == bActive)	return;
//	activeStartFly = bActive;
//
//	if (startFlyFXComponent)
//	{
//		if (bActive)
//		{
//			startFlyFXComponent->Activate();
//		}
//		else
//		{
//			startFlyFXComponent->Deactivate();
//		}
//	}
//}

////////////라인트레이스를 쏴서 일정 거리 이하가 됐을 때 착지 애니메이션 실행/////////////////////

bool ASpaceshipPawn::CheckLanding()
{
	UE_LOG(LogTemp, Warning, TEXT("checkLanding"))

	if (!IsLocallyControlled())	return false;
	FVector start = GetActorLocation();
	FVector end = start - FVector(0, 0, LandingDistance);

	FHitResult hitResult;
	FCollisionQueryParams params;
	params.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(hitResult, start, end, ECC_Visibility, params))
	{
		DrawDebugLine(GetWorld(), start, end, FColor::Magenta, false, 1.0f, 0, 20.0f);

		float distanceToGround = FVector::Dist(GetActorLocation(), hitResult.Location);

		if (distanceToGround < LandingDistance && SpaceshipSkeletalMesh && Anim)
		{
			bCantMove = true;
			bLanded = true;

			Server_PlayAnimMontage(Anim->legDownMontage);
			FTimerHandle handle;
			GetWorld()->GetTimerManager().SetTimer(handle, [this]() {
				Server_PlayAnimMontage(Anim->openDoorMontage);
				}, 2, false);
			return true;
		}
	}
	return false;
}

void ASpaceshipPawn::Server_PlayAnimMontage_Implementation(UAnimMontage* montageToPlay, float playRate /*= 1.0f*/, FName startSection /*= NAME_None*/)
{
	UE_LOG(LogTemp, Warning, TEXT("Server_PlayAnimMontage_Implementation"));

	NetMulticast_PlayAnimMontage(montageToPlay, playRate, startSection);
}

void ASpaceshipPawn::NetMulticast_PlayAnimMontage_Implementation(UAnimMontage* montageToPlay, float playRate /*= 1.0f*/, FName startSection /*= NAME_None*/)
{
	if (montageToPlay)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s"), *montageToPlay->GetName());
	}
	if (SpaceshipSkeletalMesh && Anim)
	{
		Anim->Montage_Play(montageToPlay, playRate, EMontagePlayReturnType::MontageLength);
		Anim->Montage_JumpToSection(startSection);
		UE_LOG(LogTemp, Warning, TEXT("Landing Animation Played on All Clients"));
	}
	else
	{
		FString output;
		output = Anim == nullptr ? TEXT("NULL") : *Anim->GetName();
		UE_LOG(LogTemp, Warning, TEXT("%s"), *output);
	}
}

void ASpaceshipPawn::MoveUpAnim()
{
	if (SpaceshipSkeletalMesh)
	{
		UE_LOG(LogTemp, Warning, TEXT("MoveUpAnim"))

			if (Anim)
			{
				UE_LOG(LogTemp, Warning, TEXT("legUpAnimMontage"))
				Server_CloseDoorMontageSetting();
				Server_PlayAnimMontage(Anim->closeDoorMontage);
				FTimerHandle handle;
				GetWorld()->GetTimerManager().SetTimer(handle, [this]() {
					Server_PlayAnimMontage(Anim->legUpMontage, 1, TEXT("LegUpStartSection")); 
					}, 1, false);
			}
	}
}

void ASpaceshipPawn::Server_CloseDoorMontageSetting_Implementation()
{
	NetMulticast_CloseDoorMontageSetting();
}

void ASpaceshipPawn::NetMulticast_CloseDoorMontageSetting_Implementation()
{
	Anim->Montage_SetPlayRate(Anim->legUpMontage, -1);
	Anim->Montage_SetPlayRate(Anim->closeDoorMontage, -1);
}

void ASpaceshipPawn::Server_UpdateTransform_Implementation(FVector newLocation, FRotator newRotation)
{
	SetActorLocationAndRotation(newLocation, newRotation);
}
