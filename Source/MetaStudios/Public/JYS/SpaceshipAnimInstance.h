// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SpaceshipAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class METASTUDIOS_API USpaceshipAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeBeginPlay() override;

	UFUNCTION()
	void AnimNotify_LegAnimEnded();
	
	UFUNCTION()
	void AnimNotify_LegUpLoopNotify();

	UFUNCTION()
	void AnimNotify_openDoorEnded();

	UPROPERTY(EditAnywhere)
	class UAnimMontage* legDownMontage; 
	UPROPERTY(EditAnywhere)
	class UAnimMontage* legUpMontage;

	UPROPERTY(EditAnywhere)
	class UAnimMontage* openDoorMontage;

	UPROPERTY(EditAnywhere)
	class UAnimMontage* closeDoorMontage;

	UPROPERTY()
	class ASpaceshipPawn* Me;

	void Play_LegUpMontage();
	void Play_LegDownMontage();

	void SetLegUpMontagePlayRate();

	void Play_OpenDoor();
	void SetCloseDoorMontagePlayRate();

};
