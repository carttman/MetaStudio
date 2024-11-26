// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
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
	class UUserWidget* WBP_02;
	
	UPROPERTY(meta=(BindWidget))
	class UUserWidget* WBP_04;

	UPROPERTY(meta=(BindWidget))
	class UUserWidget* WBP_05;
	
	UPROPERTY(meta=(BindWidget))
	class UUserWidget* WBP_06;
	

	UPROPERTY(meta=(BindWidget))
	class UImage* SelectMode;
	UPROPERTY(meta=(BindWidget))
	class UImage* SelectMode2;

	UPROPERTY(meta=(BindWidget))
	class UImage* TranslateMode;
	UPROPERTY(meta=(BindWidget))
	class UImage* TranslateMode2;


	UPROPERTY(meta=(BindWidget))
	class UImage* RotateMode;
	UPROPERTY(meta=(BindWidget))
	class UImage* RotateMode2;


	UPROPERTY(meta=(BindWidget))
	class UImage* ScaleMode;
	UPROPERTY(meta=(BindWidget))
	class UImage* ScaleMode2;



	UPROPERTY(meta=(BindWidget))
	class UImage* Window0;
	UPROPERTY(meta=(BindWidget))
	class UImage* Window1;
	UPROPERTY(meta=(BindWidget))
	class UImage* Window2;
	UPROPERTY(meta=(BindWidget))
	class UImage* Window3;
	UPROPERTY(meta=(BindWidget))
	class UImage* Window4;
	UPROPERTY(meta=(BindWidget))
	class UImage* Window5;
	
	UPROPERTY(meta=(BindWidget))
	UButton* Button_w1;
	UFUNCTION()
	void OnButton_w1_Clicked();

	UPROPERTY(meta=(BindWidget))
	UButton* Button_w2;
	UFUNCTION()
	void OnButton_w2_Clicked();

	UPROPERTY(meta=(BindWidget))
	UButton* Button_w3;
	UFUNCTION()
	void OnButton_w3_Clicked();


	UPROPERTY(meta=(BindWidget))
	UButton* Button_w4;
	UFUNCTION()
	void OnButton_w4_Clicked();

	UPROPERTY(meta=(BindWidget))
	UButton* Button_w5;
	UFUNCTION()
	void OnButton_w5_Clicked();
};
