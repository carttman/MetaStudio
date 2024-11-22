// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "JSH_Rotate_GizmoY.generated.h"

class AJSH_PlayerController;
class AJSH_Player;

UCLASS()
class METASTUDIOS_API AJSH_Rotate_GizmoY : public AActor
{
	GENERATED_BODY()

	UPROPERTY()
	USceneComponent* Root_Scene;
	
	UPROPERTY()
	UStaticMeshComponent* Origin;
	UPROPERTY()
	UStaticMeshComponent* Origin2;
	UPROPERTY()
	UStaticMeshComponent* Origin3;
	UPROPERTY()
	UStaticMeshComponent* Origin4;

	
	UPROPERTY()
	UMaterial* YellowMaterial;
	UPROPERTY()
	UMaterial* GreenMaterial;
	
	// UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Body, meta = (AllowPrivateAccess = "true"))
	// UStaticMeshComponent* Selected;
	
public:	
	// Sets default values for this actor's properties
	AJSH_Rotate_GizmoY();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	
};