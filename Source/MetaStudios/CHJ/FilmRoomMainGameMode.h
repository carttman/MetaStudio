// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FilmRoomMainGameMode.generated.h"

/**
 * 
 */
UCLASS()
class METASTUDIOS_API AFilmRoomMainGameMode : public AGameModeBase
{
	GENERATED_BODY()

	virtual APawn* SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot) override;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AJSH_Player> ServerPlayer;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AMetaStudiosCharacter> ClientPlayer;
	
};
