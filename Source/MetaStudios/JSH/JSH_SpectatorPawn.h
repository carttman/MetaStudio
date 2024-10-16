// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpectatorPawn.h"
#include "GameFramework/SpectatorPawnMovement.h"
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

	
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_BackMainPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_EditMode;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_FlyMode;

	void BeginSpectator();


	virtual void BeginPlay() override;
	
public:
	UFUNCTION()
	void BackPlayer();
	UFUNCTION(NetMulticast, Reliable)
	void NetMulti_BackPlayer();

	UFUNCTION()
	void EditModeON();
	// UFUNCTION(NetMulticast, Reliable)
	// void NetMulti_EditMode();

	UFUNCTION()
	void EditModeOFF();

	USpectatorPawnMovement* SpectatorPawnMovementComponent;
	
	APlayerController* SpectatorController;
};
