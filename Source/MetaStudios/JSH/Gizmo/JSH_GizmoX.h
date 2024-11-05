// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "JSH_GizmoX.generated.h"

UCLASS()
class METASTUDIOS_API AJSH_GizmoX : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AJSH_GizmoX();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:

	UFUNCTION()
	void Teleport_to_EditorActor(FRotator TelRotator);

};
