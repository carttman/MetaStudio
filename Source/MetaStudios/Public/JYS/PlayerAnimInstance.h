// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class METASTUDIOS_API UPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeBeginPlay() override;

	UPROPERTY(EditAnywhere)
	class UAnimMontage* pickUpMontage;

	UPROPERTY()
	class AMetaStudiosCharacter* Me;

	void Play_pickUpMontage();
	
};
