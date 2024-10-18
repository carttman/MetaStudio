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

	
	virtual void BeginPlay() override;
	
public:

	
	


	//// MainPlayer <-> SpectatorPawn ////=========================
	UFUNCTION()
	void BackPlayer();
	UFUNCTION(NetMulticast, Reliable)
	void NetMulti_BackPlayer();
	//// ================================= MainPlayer <-> SpectatorPawn ////



	
	//// Editor ////=================================================   
	USpectatorPawnMovement* SpectatorPawnMovementComponent;
	APlayerController* OriginController;
	
	UFUNCTION()
	void EditModeON();
	UFUNCTION()
	void EditModeOFF();
	UFUNCTION()
	void EnableEdit();
	UFUNCTION()
	void DisableEdit();
	////================================================= Editor ////

};
