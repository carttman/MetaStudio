// Fill out your copyright notice in the Description page of Project Settings.


#include "../JSH/BetaPl/JSH_TheaterSpawnActor.h"

// Sets default values
AJSH_TheaterSpawnActor::AJSH_TheaterSpawnActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	Root_Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Root_Capsule"));
	if (Root_Capsule)
	{
		RootComponent = Root_Capsule;
		Root_Capsule->SetSimulatePhysics(true);
	}

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AJSH_TheaterSpawnActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AJSH_TheaterSpawnActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}



void AJSH_TheaterSpawnActor::SET_JOwner(AActor* me)
{
	Server_SET_JOwner(me);
}

void AJSH_TheaterSpawnActor::Server_SET_JOwner_Implementation(AActor* me)
{
	Netmulti_SET_JOwner(me);
}


void AJSH_TheaterSpawnActor::Netmulti_SET_JOwner_Implementation(AActor* me)
{

	SetOwner(JOwner);
}



