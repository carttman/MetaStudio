// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "JSH_Record_UI.generated.h"

/**
 * 
 */
UCLASS()
class METASTUDIOS_API UJSH_Record_UI : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	UPROPERTY(meta=(BindWidget))
	class UCanvasPanel* Root_CanvasPanel;
	
	UPROPERTY(meta=(BindWidget))
	class UImage* BackGround;

	UPROPERTY(meta=(BindWidget))
	class UImage* BackGround2;
	

	UPROPERTY(meta=(BindWidget))
	class UImage* Rec;


	UPROPERTY(meta=(BindWidget))
	class UTextBlock* Speed;

	UPROPERTY()
	class AJSH_PlayerController* JPlayerController;

	UPROPERTY()
	class AJSH_Player* OriginPlayer;

	UFUNCTION()
	void StartRecrod_Anim();

	UFUNCTION()
	void StopRecrod_Anim();
	
	UFUNCTION()
	void Speed_Update(FText NewText);

	UPROPERTY(editanywhere, blueprintReadWrite)
	bool StartRecord = false;
	//
	UPROPERTY(editanywhere, blueprintReadWrite)
	float Speed_Value = 0.0f;
};
