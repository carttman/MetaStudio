// Fill out your copyright notice in the Description page of Project Settings.


#include "JYS/SpaceshipCharacter.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

// Sets default values
ASpaceshipCharacter::ASpaceshipCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 300.0f;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArm);
}

// Called when the game starts or when spawned
void ASpaceshipCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASpaceshipCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FTransform ttt = FTransform(this->GetControlRotation());
	Direction = ttt.TransformVector(Direction);
	Direction.Z = 0;
	Direction.Normalize();
	// 2. TransformDirection기능 이용해서 방향을 만들어서
	// 3. 그 방향으로 이동
	this->AddMovementInput(Direction);
	Direction = FVector::ZeroVector;
}

// Called to bind functionality to input
void ASpaceshipCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(IA_SpaceshipLook, ETriggerEvent::Triggered, this, &ASpaceshipCharacter::OnMyActionLook);
		EnhancedInputComponent->BindAction(IA_SpaceshipMove, ETriggerEvent::Triggered, this, &ASpaceshipCharacter::OnMyActionMove);
	}


}

void ASpaceshipCharacter::OnMyActionMove(const FInputActionValue& Value)
{
	FVector2D v = Value.Get<FVector2D>();
	Direction.X = v.X;
	Direction.Y = v.Y;
	Direction.Normalize();
}

void ASpaceshipCharacter::OnMyActionLook(const FInputActionValue& Value)
{
	FVector2D v = Value.Get<FVector2D>();

	this->AddControllerPitchInput(-v.Y);
	this->AddControllerYawInput(v.X);
}

