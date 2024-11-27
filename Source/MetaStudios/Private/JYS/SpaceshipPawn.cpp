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
#include "Components/BoxComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/ArrowComponent.h"
#include <Kismet/KismetMathLibrary.h>

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

	startFlyFXComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("startFlyEffectComponent"));
	startFlyFXComponent->SetupAttachment(SpaceshipSkeletalMesh);

	startFlyFXComponent2 = CreateDefaultSubobject<UNiagaraComponent>(TEXT("startFlyEffectComponent2"));
	startFlyFXComponent2->SetupAttachment(SpaceshipSkeletalMesh);

	UIBox = CreateDefaultSubobject<UBoxComponent>(TEXT("UIBox"));
	UIBox->SetupAttachment(SpaceshipSkeletalMesh);

	UIBox->SetGenerateOverlapEvents(true);
	UIBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);

	//ExitPosition = CreateDefaultSubobject<UArrowComponent>(TEXT("ExitPosition"));
	//ExitPosition->SetupAttachment(SpaceshipSkeletalMesh);
	//ExitPosition->SetRelativeLocation(FVector(0.0f, -2700.0f, 380.0f));
	//(X = -2370.000000, Y = 30.000000, Z = 380.000000)

	ExitPoint = CreateDefaultSubobject<USceneComponent>(TEXT("ExitPoint"));
	ExitPoint->SetupAttachment(SpaceshipSkeletalMesh);
	ExitPoint->SetRelativeLocation(FVector(0.0f, -2000.0f, 200.0f));
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
		Anim->Play_OpenDoor();
	}
	ActivateStartFly(false);

	if (UIBox)
	{
		UIBox->OnComponentBeginOverlap.AddDynamic(this, &ASpaceshipPawn::OnUIBoxBeginOverlap);
		UIBox->OnComponentEndOverlap.AddDynamic(this, &ASpaceshipPawn::OnUIBoxEndOverlap);

	}
}


// Called every frame 
void ASpaceshipPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!IsLocallyControlled())	return;

	if (SpaceshipSkeletalMesh != nullptr)
	{
		//FVector SocketLocation = GetActorLocation() + ExitPosition->GetRelativeLocation();			//SpaceshipSkeletalMesh->GetSocketLocation(FName("ExitSpaceship"));
		DrawDebugCapsule(GetWorld(), ExitPoint->GetComponentLocation(), 50.0f, 100.0f, FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(), FColor::Red, false, 10.0f);

	}

	/*
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
	*/

	if (IsLocallyControlled())
	{
		if (MoveStop == true)
		{			
			Server_UpdateRollbackSpaceship();
		}
	}




	//GEngine->AddOnScreenDebugMessage(0, DeltaTime, FColor::Green, FString::Printf(TEXT("bIsMoving : %d"), bIsMoving));
}

// Called to bind functionality to input
void ASpaceshipPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//PlayerInputComponent->BindAction("ExitSpaceship", IE_Pressed, this, &ASpaceshipPawn::ExitSpaceship);

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

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(SpaceshipLook, ETriggerEvent::Triggered, this, &ASpaceshipPawn::OnMyActionLook);
		EnhancedInputComponent->BindAction(SpaceshipMove, ETriggerEvent::Triggered, this, &ASpaceshipPawn::OnMyActionMove);
		EnhancedInputComponent->BindAction(SpaceshipMove, ETriggerEvent::Completed, this, &ASpaceshipPawn::StopMove);
		EnhancedInputComponent->BindAction(SpaceshipMoveUp, ETriggerEvent::Triggered, this, &ASpaceshipPawn::OnMoveUp);
		EnhancedInputComponent->BindAction(SpaceshipMoveDown, ETriggerEvent::Triggered, this, &ASpaceshipPawn::OnMoveDown);

	}
}

void ASpaceshipPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASpaceshipPawn, MoveStop);
	//DOREPLIFETIME(ASpaceshipPawn, bIsMoving);

}

bool ASpaceshipPawn::CanPlayerEnter(AMetaStudiosCharacter* targetCharacter)
{
	// �÷��̾�� ���ּ� ������ ����

	if (targetCharacter == nullptr)	return false;

	//float distance = GetDistanceTo(targetCharacter);
	//return distance <= 1000.0f;

	if (SpaceshipSkeletalMesh)
	{
		FVector SocketLocation = SpaceshipSkeletalMesh->GetSocketLocation(FName("EnterSpaceship"));

		float Distance = FVector::Dist(SocketLocation, targetCharacter->GetActorLocation());

		return Distance <= 1000.0f;

	}
	return false;
}

void ASpaceshipPawn::ExitSpaceship()
{
	if (!HasAuthority() && IsLocallyControlled()/* && !bIsMoving*/)
	{		
		UE_LOG(LogTemp, Warning, TEXT("ExitSpaceShip : %s"), *GetActorLocation().ToString())
		Server_ExitSpaceship();
	}
}

void ASpaceshipPawn::Server_ExitSpaceship_Implementation()
{
	if (player == nullptr)	return;	

	// ���ּ� ���� ��ġ ��������
	FVector SocketLocation = FVector::ZeroVector;
	FRotator SocketRotation = FRotator::ZeroRotator;
	FVector Direction;
	if (SpaceshipSkeletalMesh)
	{	
		SocketLocation = ExitPoint->GetComponentLocation();
		SocketRotation = GetActorRotation();
		DrawDebugCapsule(GetWorld(), SocketLocation, 100.0f, 200.0f, FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(), FColor::Green, false, 20.0f);


		//UE_LOG(LogTemp, Warning, TEXT("ExitSpaceShip 2 : %s"), *GetActorLocation().ToString())		
		player->SetActorLocation(SocketLocation);
		//UE_LOG(LogTemp, Warning, TEXT("ExitSpaceShip 3 : %s"), *player->GetActorLocation().ToString())		
	}
	else
	{
		// ������ ���� ��� �⺻ ���ּ� ��ġ ���
		SocketLocation = GetActorLocation();
		SocketRotation = GetActorRotation();
		//UE_LOG(LogTemp, Warning, TEXT("ExitSpaceShip EX : %s"), *GetActorLocation().ToString())		
	}
	
	// �÷��̾� ��Ʈ�ѷ��� Possess ó��
	GetController()->Possess(player);

	//UE_LOG(LogTemp, Warning, TEXT("===== ExitShipInfo : %s : %s"), *SocketLocation.ToString(), *SocketRotation.ToString());
	// Multicast ȣ��
	NetMulticast_ExitSpaceship(SocketLocation, SocketRotation);
	bExistRider = false;
}

void ASpaceshipPawn::NetMulticast_ExitSpaceship_Implementation(FVector ExitLocation, FRotator ExitRotation)
{
	//UE_LOG(LogTemp, Warning, TEXT("ExitSpaceShip 4 : %s"), *ExitLocation.ToString())		
	if (player)
	{		
		//UE_LOG(LogTemp, Warning, TEXT("ExitSpaceShip 5 : %s"), *GetActorLocation().ToString())
		//UE_LOG(LogTemp, Warning, TEXT("ExitSpaceShip 6 : %s"), *player->GetActorLocation().ToString())
		DrawDebugCapsule(GetWorld(), ExitLocation, 50.0f, 100.0f, FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(), FColor::Blue, false, 20.0f);
		
		// �÷��̾� ����ȭ ó��
		player->SetVisibilityMesh(true);
		//player->GetMesh()->SetVisibility(true);
		//player->JetMesh->SetVisibility(true);

		// ��Ÿ ���� ó��
		Server_EndFlyEffect();
		player->ResetEnhancedInputSetting(Cast<APlayerController>(GetWorld()->GetFirstPlayerController()));
	}
	bExistRider = false;
}

void ASpaceshipPawn::ResetEnhancedInputSetting(class APlayerController* pc)
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

void ASpaceshipPawn::OnMyActionMove(const FInputActionValue& value)
{
	FVector2D v = value.Get<FVector2D>();
	Server_UpdateMoveAndRollSpaceship(v);
}

void ASpaceshipPawn::Server_OnMyActionMoveSpaceship_Implementation(bool bMove)
{
	MoveStop = bMove;
}

void ASpaceshipPawn::OnMyActionLook(const FInputActionValue& value)
{
	FVector2D v = value.Get<FVector2D>();

	//AddControllerPitchInput(-v.Y);
	//AddControllerYawInput(v.X);
}

void ASpaceshipPawn::OnMoveUp(const FInputActionValue& value)
{
	if (bCantMove)	return;
	//UE_LOG(LogTemp, Warning, TEXT("OnMoveUP"))

	Server_UpdateMoveUpDownSpaceship(true);
}

void ASpaceshipPawn::OnMoveDown(const FInputActionValue& value)
{
	if (bCantMove)	return;
	// �̹� �������̰ų� ���鿡 ������� �������� �ʰ� return;
	if (bLanded || CheckLanding()) return;

	Server_UpdateMoveUpDownSpaceship(false);
}

void ASpaceshipPawn::StopMove()
{
	Server_OnMyActionMoveSpaceship(true);
}

FRotator ASpaceshipPawn::ApplyRoll(float RollInput)
{
	FRotator CurrentRotation = GetControlRotation();

	float RollAmount = RollInput * 45.0f;
	float YawAmount = RollInput * 45.0f;

	CurrentRotation.Roll = FMath::FInterpTo(CurrentRotation.Roll, RollAmount, GetWorld()->GetDeltaSeconds(), 3.0f);

	float YawCur = CurrentRotation.Yaw;

	// P = P0 + vt
	CurrentRotation.Yaw = YawCur + 50.0f * RollInput * GetWorld()->DeltaTimeSeconds;

	GetController()->SetControlRotation(CurrentRotation);

	return CurrentRotation;
}

FRotator ASpaceshipPawn::ApplyRollBack()
{
	FRotator CurrentRotation = GetControlRotation();

	float RollAmount = CurrentRotation.Roll;
	//float YawAmount = CurrentRotation.Yaw;

	if (RollAmount == 0.0f /*&& YawAmount == 0.0f*/)	return CurrentRotation;

	//CurrentRotation.Yaw = FMath::FInterpTo(CurrentRotation.Yaw, 0.0f, GetWorld()->GetDeltaSeconds(), 2.0f);
	CurrentRotation.Roll = FMath::FInterpTo(CurrentRotation.Roll, 0.0f, GetWorld()->GetDeltaSeconds(), 3.0f);

	GetController()->SetControlRotation(CurrentRotation);

	return CurrentRotation;
}

// ������ ���� Ű ���� UI ����
void ASpaceshipPawn::OnUIBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsLocallyControlled())	return;

	if (OtherActor && OtherActor->IsA(AMetaStudiosCharacter::StaticClass()))
	{
		AMetaStudiosCharacter* targetChar = Cast<AMetaStudiosCharacter>(OtherActor);
		if (targetChar->IsLocallyPlayer() == false)	return;

		if (EnterWidgetClass && !ActiveWidget)
		{
			ActiveWidget = CreateWidget<UUserWidget>(GetWorld(), EnterWidgetClass);
			if (ActiveWidget)
			{
				ActiveWidget->AddToViewport();
			}
		}

		if (ActiveWidget)
		{
			ActiveWidget->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

void ASpaceshipPawn::OnUIBoxEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (IsLocallyControlled())	return;

	if (OtherActor && OtherActor->IsA(AMetaStudiosCharacter::StaticClass()))
	{
		if (ActiveWidget)
		{
			ActiveWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}
// ������ ���� Ű ���� UI ����

void ASpaceshipPawn::Server_UpdateTransformSpaceship_Implementation(FVector newLocation, FRotator newRotation)
{
	SetActorLocationAndRotation(newLocation, newRotation);
}

void ASpaceshipPawn::Server_UpdateMoveUpDownSpaceship_Implementation(bool bMoveUp)
{
	FVector currentLocation = GetActorLocation();
	if (bMoveUp == true)
	{
		// ���� ���
		currentLocation.Z += MovementSpeed * GetWorld()->GetDeltaSeconds();
	}
	else
	{
		// ���� �ϰ�
		currentLocation.Z -= MovementSpeed * GetWorld()->GetDeltaSeconds();
	}
	SetActorLocation(currentLocation);
	NetMulticast_UpdateMoveUpDownSpaceShip(GetActorLocation());
}

void ASpaceshipPawn::NetMulticast_UpdateMoveUpDownSpaceShip_Implementation(FVector newLocation)
{
	SetActorLocation(newLocation);
	LastLandingPosZ = GetActorLocation().Z;
	if (bLanded)
	{
		bLanded = false;
		MoveUpAnim();
		Server_StartFlyEffect();
		//bIsMoving = true;
		// ActivateStartFly(true);
	}


	//SetActorLocation(currentLocation);
}

void ASpaceshipPawn::Server_UpdateMoveAndRollSpaceship_Implementation(FVector2D inputValue)
{
	if (inputValue.X <= 0.0f)	return;

	FRotator newRot;
	direction.X = inputValue.X;
	if (direction.X != 0.0f)
	{
		newRot = ApplyRoll(inputValue.Y);
	}
	else
	{
		newRot = ApplyRollBack();
	}

	FTransform ttt = FTransform(GetControlRotation());
	direction = ttt.TransformVector(direction);
	direction.Z = 0;
	direction.Normalize();

	// p = p0 + vt
	FVector newPos = GetActorLocation() + direction * 5000.0f * GetWorld()->DeltaTimeSeconds;
	SetActorLocation(newPos);
	//AddMovementInput(direction);
	direction = FVector::ZeroVector;

	NetMulticast_UpdateMoveAndRollSpaceShip(newPos, newRot);
	Server_OnMyActionMoveSpaceship_Implementation(false);
}

void ASpaceshipPawn::NetMulticast_UpdateMoveAndRollSpaceShip_Implementation(FVector newLocation, FRotator newRot)
{
	if (IsLocallyControlled())
	{
		GetController()->SetControlRotation(newRot);
	}
	else
	{
		SetActorRotation(newRot);
	}
	SetActorLocation(newLocation);
}

void ASpaceshipPawn::Server_UpdateRollbackSpaceship_Implementation()
{
	FRotator newRot = ApplyRollBack();
	NetMulticast_UpdateRollbackSpaceShip(GetActorLocation(), newRot);
}

void ASpaceshipPawn::NetMulticast_UpdateRollbackSpaceShip_Implementation(FVector newLocation, FRotator newRot)
{
	if (IsLocallyControlled())
	{
		GetController()->SetControlRotation(newRot);
	}
	else
	{
		SetActorRotation(newRot);
	}
}

void ASpaceshipPawn::Server_StartFlyEffect_Implementation()
{
	NetMulticast_StartFlyEffect();
}

void ASpaceshipPawn::NetMulticast_StartFlyEffect_Implementation()
{
	FVector start = GetActorLocation();
	FVector end = start - FVector(0, 0, 10000.0f);

	FHitResult hitResult;
	FCollisionQueryParams params;
	params.AddIgnoredActor(this);



	float SphereRadius = 100.0f;
	float HalfSphereRadius = SphereRadius * 0.5f;

	//DrawDebugLine(GetWorld(), startLocation, endLocation, FColor::Magenta, false, 1.0f, 0, 20.0f);      

	bool HitDetected = GetWorld()->SweepSingleByChannel(hitResult, start, end, FQuat::Identity, ECC_Visibility, FCollisionShape::MakeSphere(SphereRadius), params);
	if (HitDetected)
	{
		FVector currentLocation = GetActorLocation();
		FVector newLocation = hitResult.ImpactPoint;
		newLocation.Z += HalfSphereRadius;

		//float distanceToGround = FVector::Dist(currentLocation, hitResult.Location);

		currentLocation.Z += 500.0f * GetWorld()->GetDeltaSeconds();

		/////////////////���ּ� ����� �� ����Ʈ/////////////////
		ActivateStartFly(true);

		SetActorLocation(currentLocation);
	}

	FVector CapsuleOrigin = start + (end - start) * 0.5f;
	FColor DrawColor = HitDetected ? FColor::Green : FColor::Red;

	DrawDebugCapsule(GetWorld(), hitResult.ImpactPoint, HalfSphereRadius, SphereRadius, FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(), DrawColor, false, 5.0f);
}

void ASpaceshipPawn::Server_EndFlyEffect_Implementation()
{
	NetMulticast_EndFlyEffect();
}

void ASpaceshipPawn::NetMulticast_EndFlyEffect_Implementation()
{
	ActivateStartFly(false);
}

void ASpaceshipPawn::EndFlyEffect()
{
}

void ASpaceshipPawn::ActivateStartFly(bool bActive)
{
	if (activeStartFly == bActive)	return;
	activeStartFly = bActive;

	if (startFlyFXComponent)
	{
		if (bActive)
		{
			startFlyFXComponent->Activate();
		}
		else
		{
			startFlyFXComponent->Deactivate();
		}
	}
	if (startFlyFXComponent2)
	{
		if (bActive)
		{
			startFlyFXComponent2->Activate();
		}
		else
		{
			startFlyFXComponent2->Deactivate();
		}
	}
}

////////////����Ʈ���̽��� ���� ���� �Ÿ� ���ϰ� ���� �� ���� �ִϸ��̼� ����/////////////////////

bool ASpaceshipPawn::CheckLanding()
{
	if (!IsLocallyControlled())	return false;
	UE_LOG(LogTemp, Warning, TEXT("CheckLanding"));
	FVector start = GetActorLocation();
	FVector end = start - FVector(0, 0, 100000000000);

	bool bHitResult = false;
	FHitResult hitResult;
	FCollisionQueryParams params;
	params.AddIgnoredActor(this);
	DrawDebugLine(GetWorld(), start, end, FColor::Red, false, 1.0f, 0, 20.0f);
	if (GetWorld()->LineTraceSingleByChannel(hitResult, start, end, ECC_Visibility, params))
	{
		UE_LOG(LogTemp, Warning, TEXT("LineTrace Success"));
		bHitResult = true;
		DrawDebugLine(GetWorld(), start, end, FColor::Magenta, false, 1.0f, 0, 20.0f);
		LastLandingPosZ = hitResult.ImpactPoint.Z;

		float distanceToGround = FVector::Dist(GetActorLocation(), hitResult.ImpactPoint);
		// UE_LOG(LogTemp, Warning, TEXT("dist : %f - %f"), distanceToGround, LandingDistance)
		UE_LOG(LogTemp, Warning, TEXT("DistanceToGround: %f, LandingDistance: %f"), distanceToGround, LandingDistance);
		if (distanceToGround < LandingDistance)
		{
			UE_LOG(LogTemp, Warning, TEXT("distanceToGround < LandingDistance"));
			bCantMove = true;
			bLanded = true;
			//bIsMoving = false;

			Server_PlayAnimMontage(Anim->legDownMontage);
			FTimerHandle handle;
			GetWorld()->GetTimerManager().SetTimer(handle, [this]() {
				Server_PlayAnimMontage(Anim->openDoorMontage);
				}, 2, false);
			Server_EndFlyEffect();


			return true;
		}
	}

	if (bHitResult == false && GetActorLocation().Z < LastLandingPosZ)
	{
		bCantMove = true;
		bLanded = true;

		Server_PlayAnimMontage(Anim->legDownMontage);
		FTimerHandle handle;
		GetWorld()->GetTimerManager().SetTimer(handle, [this]() {
			Server_PlayAnimMontage(Anim->openDoorMontage);
			}, 2, false);
		Server_EndFlyEffect();
		LastLandingPosZ = 1000000.0f;
		return true;
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
