// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "MetaStudios/JSH/JSH_BetaPlayer.h"
#include "JSH_VideoStart.generated.h"

/**
 * 
 */
UCLASS()
class METASTUDIOS_API UJSH_VideoStart : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta=(BindWidget))
	class UCanvasPanel* Root_CanvasPanel;
	UPROPERTY(meta=(BindWidget))
	class UImage* Guide01;
	UPROPERTY(meta=(BindWidget))
	class UImage* Guide02;
	UPROPERTY(meta=(BindWidget))
	UButton* Button_Next;
	UPROPERTY(meta=(BindWidget))
	UButton* Button_Back;
	UPROPERTY(meta=(BindWidget))
	UButton* Button_Exe;




public:
	UPROPERTY(meta=(BindWidget))
	class UImage* OverlapImage;
	
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnButton_Next();
	UFUNCTION()
	void OnButton_Back();
	UFUNCTION()
	void OnButton_Exe();

	UFUNCTION()
	void Saved_BetaPlayer(AJSH_BetaPlayer* pl);

	UPROPERTY()
	AJSH_BetaPlayer* OriginPlayer;

	
};
