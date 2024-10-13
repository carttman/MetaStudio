// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "JSH_Player.h"
#include "IWebSocket.h"
#include "JSH_OBSWebSocket.generated.h"

/**
 * 
 */
UCLASS()
class METASTUDIOS_API UJSH_OBSWebSocket : public UGameInstance
{
	GENERATED_BODY()

// public:
// 	UJSH_OBSWebSocket();
//
// 	// WebSocket 객체
// 	TSharedPtr<IWebSocket> OBSWebSocket;
//
// 	// OBS WebSocket에 연결하는 함수
// 	void ConnectToOBS();
//
// 	// // 녹화 시작 함수
// 	// UFUNCTION(BlueprintCallable)
// 	// void StartRecording();
// 	//
// 	// // 녹화 종료 함수
// 	// UFUNCTION(BlueprintCallable)
// 	// void StopRecording();
//
// protected:
// 	virtual void Init() override;  // 게임이 시작할 때 초기화
// 	virtual void Shutdown() override;  // 게임이 종료될 때 정리
//


public:
	virtual void Init() override;
	virtual void Shutdown() override;

	void OBSWebSocketConnect();
	void OBSWebSocketDisConnect();
	
	TSharedPtr<IWebSocket> WebSocket;

	// class AJSH_PlayerController* PController;

	
	class AJSH_Player* DirectorPlayer;


	void StartRecord();

	FString CreatePasswordHash(const FString& Password, const FString& Salt, const FString& Challenge);
};
