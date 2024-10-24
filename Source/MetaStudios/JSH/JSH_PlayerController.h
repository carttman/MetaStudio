// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "JSH_Player.h"
#include "GameFramework/PlayerController.h"
#include "JSH_PlayerController.generated.h"

/**
 * 
 */
UCLASS()
class METASTUDIOS_API AJSH_PlayerController : public APlayerController
{
	GENERATED_BODY()


public:
	UPROPERTY()
	class AJSH_Player* OriginPlayer;

	UFUNCTION()
	void SaveOriginCharacter();
	UFUNCTION(NetMulticast, Reliable)
	void NetMulti_SaveOriginCharacter();


	void PlayerViewOnOff();


	//// Record System /// ====================================================
	class AJSH_Player* DirectorPlayer;
	
	void StartRecord();
	
	FProcHandle PH;
	
	// 녹화 모드 On, Off 제어
	bool Recording = false;
	
	FString VideoFileName;

	
	void ConvertMKVToMP4();
	//// ======================
	
};
