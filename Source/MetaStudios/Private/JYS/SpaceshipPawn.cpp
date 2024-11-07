// Fill out your copyright notice in the Description page of Project Settings.


#include "JYS/SpaceshipPawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Animation/AnimInstance.h"

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
}

// Called when the game starts or when spawned
void ASpaceshipPawn::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* playerController = Cast<APlayerController>(GetController());
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

void ASpaceshipPawn::OnMyActionMove(const FInputActionValue& value)
{
	FVector2D v = value.Get<FVector2D>();
	direction.X = v.X;
	direction.Y = v.Y;
	direction.Z = v.Y;
	direction.Normalize();

}

void ASpaceshipPawn::OnMyActionLook(const FInputActionValue& value)
{
	FVector2D v = value.Get<FVector2D>();

	AddControllerPitchInput(-v.Y);
	AddControllerYawInput(v.X);

}

void ASpaceshipPawn::OnMoveUp(const FInputActionValue& value)
{
	FVector currentLocation = GetActorLocation();
	// 수직 상승
	currentLocation.Z += MovementSpeed * GetWorld()->GetDeltaSeconds();

	SetActorLocation(currentLocation);
}

void ASpaceshipPawn::OnMoveDown(const FInputActionValue& value)
{
	FVector currentLocation = GetActorLocation();
	// 수직 하강
	currentLocation.Z -= MovementSpeed * GetWorld()->GetDeltaSeconds();
	SetActorLocation(currentLocation);
	FHitResult hitResult;
	FVector downTraceStart = currentLocation;
	FVector downTraceEnd = downTraceStart - FVector(0, 0, 100); // 아래 방향으로 100 유닛 트레이스

	float desiredHeight = currentLocation.Z + (MovementSpeed * GetWorld()->GetDeltaSeconds());

	// 라인 트레이스를 사용하여 바닥에 닿았는지 확인
	if (GetWorld()->LineTraceSingleByChannel(hitResult, downTraceStart, downTraceEnd, ECC_Visibility))
	{
		// 바닥에 닿았다면, 바닥의 높이를 기준으로 수직 위치 설정
		desiredHeight = FMath::Max(hitResult.Location.Z + 10.0f, currentLocation.Z); // 약간의 여유를 두기 위해 10 유닛 추가
	}

	// 새로운 위치 설정
	currentLocation.Z = desiredHeight;
	CheckLanding();
	
}


////////////라인트레이스를 쏴서 일정 거리 이하가 됐을 때 착지 애니메이션 실행 및 자동 착지/////////////////////

void ASpaceshipPawn::CheckLanding()
{
	FVector start = GetActorLocation();
	FVector end = start - FVector(0, 0, 500.0f);

	FHitResult hitResult;
	FCollisionQueryParams params;
	params.AddIgnoredActor(this);
	////////////////////500.0f 이하는 속도가 출어든다///////////////////////
	if (GetWorld()->LineTraceSingleByChannel(hitResult, start, end, ECC_Visibility, params))
	{
		DrawDebugLine(GetWorld(), start, end, FColor::Magenta, false, 1.0f, 0, 5.0f);
		float targetZ = hitResult.Location.Z;
		FVector currentLocation = GetActorLocation();

		float distanceToGround = FVector::Dist(currentLocation, hitResult.Location);
		if (distanceToGround < 500.0f)
		{
			currentLocation.Z -= 500.0f * GetWorld()->GetDeltaSeconds();
			//currentLocation.Z = FMath::FInterpTo(currentLocation.Z, targetZ, GetWorld()->GetDeltaSeconds(), 5.0f);
		}

		if (FVector::Dist(currentLocation, hitResult.Location) < 10.0f)
		{
			UE_LOG(LogTemp, Warning, TEXT("kkkkkkkkkk"));
			currentLocation.Z = targetZ;
		}
		SetActorLocation(currentLocation);
	}
}

void ASpaceshipPawn::Server_UpdateTransform_Implementation(FVector newLocation, FRotator newRotation)
{
	SetActorLocationAndRotation(newLocation, newRotation);
}

