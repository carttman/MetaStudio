// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "JSH_Scale_GizmoX.generated.h"

class AJSH_PlayerController;

UCLASS()
class METASTUDIOS_API AJSH_Scale_GizmoX : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Body, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Origin;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Body, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Selected;
	
public:	
	// Sets default values for this actor's properties
	AJSH_Scale_GizmoX();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	
public:
	UFUNCTION()
	void OnMeshClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed);

	UPROPERTY()
	AJSH_PlayerController* JPlayerController;
	
};
