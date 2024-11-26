// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Actor.h"
#include "JSH_TheaterSpawnActor.generated.h"

class AJSH_BetaPlayer;

UCLASS()
class METASTUDIOS_API AJSH_TheaterSpawnActor : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UCapsuleComponent* Root_Capsule;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* MeshComp;
	

public:	
	// Sets default values for this actor's properties
	AJSH_TheaterSpawnActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void SET_JOwner(AActor* me);
	UFUNCTION(Reliable, Server)
	void Server_SET_JOwner(AActor* me);
	UFUNCTION(Reliable, NetMulticast)
	void Netmulti_SET_JOwner(AActor* me);

	UPROPERTY()
	AActor* JOwner;

};
