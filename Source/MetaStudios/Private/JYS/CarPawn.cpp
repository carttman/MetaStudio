// Fill out your copyright notice in the Description page of Project Settings.


#include "JYS/CarPawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputSubsystems.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputComponent.h"

// Sets default values
ACarPawn::ACarPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DefaultScene = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneComp"));
	SetRootComponent(DefaultScene);

	CarMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CarMesh"));
	CarMesh->SetupAttachment(DefaultScene);

	
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(CarMesh);
	SpringArm->TargetArmLength = 300.0f;
	
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArm);
}

// Called when the game starts or when spawned
void ACarPawn::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ACarPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//if (!MovementDirection.IsZero())
	//{
	//	const FVector newLocation = GetActorLocation() + (MovementDirection * DeltaTime * MovementSpeed);
	//	SetActorLocation(newLocation);
	//}

	FTransform trans = FTransform(this->GetControlRotation());
	direction = trans.TransformVector(direction);
	direction.Z = 0;
	direction.Normalize();

	AddMovementInput(direction);
	direction = FVector::ZeroVector;
}

// Called to bind functionality to input
void ACarPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// 매핑이 위로 쌓이기 떄문에 키가 겹쳐서 안될 수 있음 그래서 매핑콘테스트를 클리어해주고 AddMappingContext 해줘야함
			Subsystem->ClearAllMappings();
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
	PlayerInputComponent->BindAction("ExitCar", IE_Pressed, this, &ACarPawn::ExitCar);


	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(CarLook, ETriggerEvent::Triggered, this, &ACarPawn::OnMyActionLook);
		EnhancedInputComponent->BindAction(CarMove, ETriggerEvent::Triggered, this, &ACarPawn::OnMyActionMove);
	}

}

bool ACarPawn::CanPlayerEnterCar()
{
	// 플레이어와 우주선 사이의 간격
	APlayerController* playerController = Cast<APlayerController>(GetWorld()->GetFirstPlayerController());
	if (!playerController) return false;

	APawn* CarPawn = playerController->GetPawn();
	if (!CarPawn) return false;

	float distance = GetDistanceTo(CarPawn);
	return distance <= 1000.0f;
}

void ACarPawn::ExitCar()
{
	APlayerController* characterController = Cast<APlayerController>(GetWorld()->GetFirstPlayerController());

	if (characterController && player)
	{
		FVector carLoc = GetActorLocation();
		FRotator carRot = GetActorRotation();

		FVector offset = carRot.RotateVector(FVector(200.0f, 0.0f, 0.0f));
		FVector playerSpawnLocation = carLoc + offset;

		player->SetActorLocation(playerSpawnLocation);
		player->SetActorRelativeRotation(carRot);
		characterController->Possess(player);
		player->GetMesh()->SetVisibility(true);
	}
}

void ACarPawn::OnMyActionMove(const FInputActionValue& value)
{
	FVector2D v = value.Get<FVector2D>();
	direction.X = v.X;
	direction.Y = v.Y;
	direction.Normalize();
}

void ACarPawn::OnMyActionLook(const FInputActionValue& value)
{
	FVector2D v = value.Get<FVector2D>();

	AddControllerPitchInput(-v.Y * 0.5f);
	AddControllerYawInput(v.X * 0.5f);
}



