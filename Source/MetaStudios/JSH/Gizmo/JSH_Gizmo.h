// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "JSH_Gizmo.generated.h"

UCLASS()
class METASTUDIOS_API AJSH_Gizmo : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USceneComponent* RootScence;

	// Translate
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UChildActorComponent* Translate_Box;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UChildActorComponent* Translate_X;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UChildActorComponent* Translate_Y;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UChildActorComponent* Translate_Z;
	
	// Sclae
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UChildActorComponent* Scale_Box;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UChildActorComponent* Scale_X;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UChildActorComponent* Scale_Y;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UChildActorComponent* Scale_Z;
	
public:	
	// Sets default values for this actor's properties
	AJSH_Gizmo();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void ScaleMode();

	UFUNCTION()
	void TranslateMode();

};
