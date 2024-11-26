// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "JSH_Preset.generated.h"

UCLASS()
class METASTUDIOS_API AJSH_Preset : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AJSH_Preset();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	UFUNCTION()
	void Hidden_On_Off();
	UFUNCTION(Server, Reliable)
	void Server_Hidden_On_Off();
	UFUNCTION(Server, Reliable)
	void NetMulti_Hidden_On_Off();
	UPROPERTY(replicated)
	bool B_Hidden = false;;


};
