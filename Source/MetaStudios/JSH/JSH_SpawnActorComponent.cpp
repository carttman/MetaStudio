// Fill out your copyright notice in the Description page of Project Settings.


#include "JSH_SpawnActorComponent.h"

#include "EngineUtils.h"
#include "Animation/SkeletalMeshActor.h"

// Sets default values for this component's properties
UJSH_SpawnActorComponent::UJSH_SpawnActorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UJSH_SpawnActorComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
	UE_LOG(LogTemp, Error, TEXT("Component beginplay"));
	
	
}


// Called every frame
void UJSH_SpawnActorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	//UE_LOG(LogTemp, Error, TEXT("Component tick"));
}
