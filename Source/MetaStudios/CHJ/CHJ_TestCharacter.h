// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CHJ_TestCharacter.generated.h"

UCLASS()
class METASTUDIOS_API ACHJ_TestCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACHJ_TestCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UFirebaseComponent* FirebaseComponent;
public:
	UFUNCTION(BlueprintImplementableEvent, Category="Firebase")
	void SetStreamUrl(const FString& url);
};
