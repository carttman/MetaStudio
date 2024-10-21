// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CharacterController.generated.h"

/**
 * 
 */
UCLASS()
class METASTUDIOS_API ACharacterController : public APlayerController
{
	GENERATED_BODY()
	
public:
	
	class AMetaStudiosCharacter* characterPlayer;

	void SaveCharacter();
};
