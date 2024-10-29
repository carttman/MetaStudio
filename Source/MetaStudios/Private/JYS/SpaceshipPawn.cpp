// Fill out your copyright notice in the Description page of Project Settings.


#include "JYS/SpaceshipPawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/SkeletalMeshComponent.h"

// Sets default values
ASpaceshipPawn::ASpaceshipPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//DefaultScene = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneComp"));
	//SetRootComponent(DefaultScene);
	
	SpaceshipMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SpaceshipMesh"));
	RootComponent = SpaceshipMesh;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(SpaceshipMesh);
	SpringArm->TargetArmLength = 300.0f;

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
			// 매핑이 위로 쌓이기 떄문에 키가 겹쳐서 안될 수 있음 그래서 매핑콘테스트를 클리어해주고 AddMappingContext 해줘야함
			Subsystem->ClearAllMappings();
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(SpaceshipLook, ETriggerEvent::Triggered, this, &ASpaceshipPawn::OnMyActionLook);
		EnhancedInputComponent->BindAction(SpaceshipMove, ETriggerEvent::Triggered, this, &ASpaceshipPawn::OnMyActionMove);
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

		FVector offset = spaceshipRot.RotateVector(FVector(200.0f, 0.0f, 0.0f));
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
			// 매핑이 위로 쌓이기 떄문에 키가 겹쳐서 안될 수 있음 그래서 매핑콘테스트를 클리어해주고 AddMappingContext 해줘야함
			Subsystem->ClearAllMappings();
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
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

void ASpaceshipPawn::Server_UpdateTransform_Implementation(FVector newLocation, FRotator newRotation)
{
	SetActorLocationAndRotation(newLocation, newRotation);
}

