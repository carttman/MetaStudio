// Fill out your copyright notice in the Description page of Project Settings.


#include "../JSH/JSH_Preset.h"

#include "Net/UnrealNetwork.h"

// Sets default values
AJSH_Preset::AJSH_Preset()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AJSH_Preset::BeginPlay()
{
	Super::BeginPlay();
	
}

void AJSH_Preset::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AJSH_Preset,B_Hidden);
	DOREPLIFETIME(AJSH_Preset,B_Hidden2);
}

// Called every frame
void AJSH_Preset::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AJSH_Preset::Hidden_On_Off()
{
	UE_LOG(LogTemp, Warning, TEXT("p4"));
	Server_Hidden_On_Off();
}


void AJSH_Preset::Server_Hidden_On_Off_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("p5"));

	NetMulti_Hidden_On_Off();
}

void AJSH_Preset::NetMulti_Hidden_On_Off_Implementation()
{
	if (B_Hidden)
	{
		SetActorHiddenInGame(true);
	}
	else
	{
		SetActorHiddenInGame(false);	
	}

	B_Hidden = !B_Hidden;
	UE_LOG(LogTemp, Warning, TEXT("p6"));
}



void AJSH_Preset::Hidden_On_Off2()
{
	Server_Hidden_On_Off2();
}

void AJSH_Preset::Server_Hidden_On_Off2_Implementation()
{
	NetMulti_Hidden_On_Off2();
}

void AJSH_Preset::NetMulti_Hidden_On_Off2_Implementation()
{
	if (B_Hidden2)
	{
		SetActorHiddenInGame(true);
	}
	else
	{
		SetActorHiddenInGame(false);	
	}

	B_Hidden2 = !B_Hidden2;
	UE_LOG(LogTemp, Warning, TEXT("p6"));
}
