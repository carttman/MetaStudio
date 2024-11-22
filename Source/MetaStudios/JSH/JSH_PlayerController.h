// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "JSH_Player.h"
#include "GameFramework/PlayerController.h"
#include "JSH_PlayerController.generated.h"

/**
 * 
 */

class AJSH_Player;

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
	AJSH_Player* DirectorPlayer;
	
	void StartRecord();
	
	FProcHandle PH;
	
	// 녹화 모드 On, Off 제어
	bool Recording = false;
	
	FString VideoFileName = TEXT("A");

	
	void ConvertMKVToMP4();
	//// ======================
	bool test = true;

#pragma region Editor Actor
	UFUNCTION()
	void SaveEditorActor(AJSH_Editor_SpawnActor* ClickedActor);
	UFUNCTION(NetMulticast, Reliable)
	void NetMulti_SaveEditorActor(AJSH_Editor_SpawnActor* ClickedActor);

	UPROPERTY()
	class AJSH_Editor_SpawnActor* Editor_SpawnActor;

	UFUNCTION()
	void Destroy_EditorActor();
	UFUNCTION(NetMulticast, Reliable)
	void NetMulti_Destroy_SaveEditorActor();
	
#pragma endregion



};
