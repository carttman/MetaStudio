// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpectatorPawn.h"
#include "JSH_SpectatorPawn.generated.h"

/**
 * 
 */
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

UCLASS()
class METASTUDIOS_API AJSH_SpectatorPawn : public ASpectatorPawn
{
	GENERATED_BODY()


	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* StartRecord;
	
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
};
