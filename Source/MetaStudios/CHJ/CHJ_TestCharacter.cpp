// Fill out your copyright notice in the Description page of Project Settings.


#include "../CHJ/CHJ_TestCharacter.h"

#include "Component/FirebaseComponent.h"

// Sets default values
ACHJ_TestCharacter::ACHJ_TestCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FirebaseComponent = CreateDefaultSubobject<UFirebaseComponent>(TEXT("FirebaseComponent"));
}

// Called when the game starts or when spawned
void ACHJ_TestCharacter::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ACHJ_TestCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACHJ_TestCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

