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
	class UImage* Center;

	UPROPERTY(meta=(BindWidget))
	class UImage* Line;
	

	UPROPERTY(meta=(BindWidget))
	class UImage* Rec_Circle;
	UPROPERTY(meta=(BindWidget))
	class UImage* Rec_WhiteCircle;
	UPROPERTY(meta=(BindWidget))
	class UImage* Rec_Text;

	//////
	UPROPERTY(meta=(BindWidget))
	class UImage* Resolation_Box;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* Resolation_Text;
	////

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
	void Speed_Update(float speedUpdate);

	UPROPERTY(editanywhere, blueprintReadWrite)
	bool StartRecord = false;
	//


	UPROPERTY(meta=(BindWidget))
	class UImage* Speed_Icon;
	UPROPERTY(meta=(BindWidget))
	class UImage* Speed_Box;
	UPROPERTY(meta=(BindWidget))
	class UImage* Speed_MinError;
	UPROPERTY(editanywhere, blueprintReadWrite)
	float Speed_Value = 0.0f;


	UPROPERTY(meta=(BindWidget))
	class UImage* Zoom_Icon;
	UPROPERTY(meta=(BindWidget))
	class UImage* Zoom_Box;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* Zoom;
	UFUNCTION()
	void Zoom_Update(float zoomupdate);
	UFUNCTION()
	void Reset_Zoom_Update();

	UPROPERTY(meta=(BindWidget))
	class UImage* Zoom_MinError;
	UPROPERTY(meta=(BindWidget))
	class UImage* Zoom_MaxError;


	UPROPERTY(meta=(BindWidget))
	class UImage* Sensitivity_Icon;
	UPROPERTY(meta=(BindWidget))
	class UImage* Sensitivity_Box;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* Sensitivity;
	UPROPERTY(meta=(BindWidget))
	class UImage* Sensitivity_MinError;
	UPROPERTY(meta=(BindWidget))
	class UImage* Sensitivity_MaxError;

	UFUNCTION()
	void Sensitivity_Update(float Mouseupdate);


	UPROPERTY(meta=(BindWidget))
	class UImage* CameraUI_Rotate_Icon;
	UPROPERTY(meta=(BindWidget))
	class UImage* CameraUI_Rotate_Box;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* CameraUI_Rotate;
	UFUNCTION()
	void CameraUI_Rotate_Update(float Cameraupdate);



	UPROPERTY(meta=(BindWidget))
	class UImage* Menue_Icon;
	UPROPERTY(meta=(BindWidget))
	class UImage* Menue_Text;


	UPROPERTY(meta=(BindWidget))
	class UImage* Battery_Icon;
};
