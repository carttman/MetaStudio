// Fill out your copyright notice in the Description page of Project Settings.


#include "JYS/SpaceshipPawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

// Sets default values
ASpaceshipPawn::ASpaceshipPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpaceshipMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SpaceshipMesh"));
	RootComponent= SpaceshipMesh;


	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 300.0f;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArm);
}

// Called when the game starts or when spawned
void ASpaceshipPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame 
void ASpaceshipPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!MovementDirection.IsZero())
	{
		const FVector newLocation = GetActorLocation() + (MovementDirection * DeltaTime * MovementSpeed);
		SetActorLocation(newLocation);
	}
}

// Called to bind functionality to input
void ASpaceshipPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ASpaceshipPawn::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ASpaceshipPawn::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("MoveUp"), this, &ASpaceshipPawn::MoveUp);
}

bool ASpaceshipPawn::CanPlayerEnter()
{
	// 플레이어와 우주선 사이의 간격
	APlayerController* playerController = Cast<APlayerController>(GetWorld()->GetFirstPlayerController());
	if (!playerController) return false;

	APawn* SpaceshipPawn = playerController->GetPawn();
	if(!SpaceshipPawn) return false;

	float distance = GetDistanceTo(SpaceshipPawn);
	return distance <= 1000.0f;
}

void ASpaceshipPawn::MoveForward(float value)
{
	MovementDirection.X = FMath::Clamp(value, -1.0f, 1.0f);
}

void ASpaceshipPawn::MoveRight(float value)
{
	MovementDirection.Y = FMath::Clamp(value, -1.0f, 1.0f);
}

void ASpaceshipPawn::MoveUp(float value)
{
	MovementDirection.Z = FMath::Clamp(value, -1.0f, 1.0f);
}

