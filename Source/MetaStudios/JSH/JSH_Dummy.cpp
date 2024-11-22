// Fill out your copyright notice in the Description page of Project Settings.


#include "../JSH/JSH_Dummy.h"

// Sets default values
AJSH_Dummy::AJSH_Dummy()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AJSH_Dummy::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AJSH_Dummy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AJSH_Dummy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

