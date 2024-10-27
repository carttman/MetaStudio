// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "JSH_Editor_MainUI.generated.h"

/**
 * 
 */
UCLASS()
class METASTUDIOS_API UJSH_Editor_MainUI : public UUserWidget
{
	GENERATED_BODY()


public:
	virtual void NativeConstruct() override;

	UPROPERTY(meta=(BindWidget))
	class UCanvasPanel* Root_CanvasPanel;
	
	UPROPERTY(meta=(BindWidget))
	class UUserWidget* WBP_01;

	UPROPERTY(meta=(BindWidget))
	class UImage* Window_Left;

	
};
