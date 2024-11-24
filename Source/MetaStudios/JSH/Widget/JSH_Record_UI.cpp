// Fill out your copyright notice in the Description page of Project Settings.


#include "../JSH/Widget/JSH_Record_UI.h"
#include "../JSH/JSH_PlayerController.h"
#include "../JSH/JSH_Player.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UJSH_Record_UI::NativeConstruct()
{
	Super::NativeConstruct();

	
	JPlayerController = Cast<AJSH_PlayerController>(GetWorld()->GetFirstPlayerController());
	if (JPlayerController)
	{
		JPlayerController->bEnableTouchEvents = false;
	}

	OriginPlayer = Cast<AJSH_Player>(JPlayerController->GetPawn());

	Rec_Circle->SetVisibility(ESlateVisibility::Hidden);
	Speed_MinError->SetVisibility(ESlateVisibility::Hidden);
	Zoom_MinError->SetVisibility(ESlateVisibility::Hidden);
	Zoom_MaxError->SetVisibility(ESlateVisibility::Hidden);
	Sensitivity_MaxError->SetVisibility(ESlateVisibility::Hidden);
	Sensitivity_MinError->SetVisibility(ESlateVisibility::Hidden);
}

void UJSH_Record_UI::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// if (OriginPlayer)
	// {
	// 	Speed_Value  = OriginPlayer->MaxFlySpeed_C;
	// }


}


void UJSH_Record_UI::StartRecrod_Anim()
{
	Rec_Circle->SetVisibility(ESlateVisibility::Visible);
	Rec_WhiteCircle->SetVisibility(ESlateVisibility::Hidden);
	Center->SetVisibility(ESlateVisibility::Hidden);


	StartRecord = true;
}

void UJSH_Record_UI::StopRecrod_Anim()
{
	Rec_Circle->SetVisibility(ESlateVisibility::Hidden);
	Rec_WhiteCircle->SetVisibility(ESlateVisibility::Visible);
	Center->SetVisibility(ESlateVisibility::Visible);

	StartRecord = false;
}

void UJSH_Record_UI::Speed_Update(float speedUpdate)
{
	//Speed->SetText(FText::AsNumber(Speed_Value));

	Speed_Value = speedUpdate;

	if (Speed_Value < -1.0f)
	{
		Speed_Value = -1.0f;
		Speed->SetText(FText::FromString(TEXT("Stop")));
		Speed_MinError->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		Speed_MinError->SetVisibility(ESlateVisibility::Hidden);
		int32 IntSpeed = FMath::FloorToInt(Speed_Value);
		Speed->SetText(FText::AsNumber(IntSpeed));
	}
}

void UJSH_Record_UI::Zoom_Update(float zoomupdate)
{
	// int32 IntZoom = static_cast<int32>(zoomupdate);
	// Zoom->SetText(FText::AsNumber(IntZoom));


	float adjustedZoom = zoomupdate - 90;
	float normalizedZoom = (adjustedZoom / -80.0f) * 100.0f;
	int32 IntZoom = FMath::RoundToInt(normalizedZoom);

	if (IntZoom <= -37)
	{
		Zoom_MinError->SetVisibility(ESlateVisibility::Visible);
		Zoom->SetText(FText::FromString(TEXT("Min")));
	}
	else if (IntZoom >= 100)
	{
		Zoom_MaxError->SetVisibility(ESlateVisibility::Visible);
		Zoom->SetText(FText::FromString(TEXT("Max")));
	}
	// else if (IntZoom == 0)
	// {
	// 	Zoom->SetText(FText::FromString(TEXT("Default")));
	// }
	else
	{
		Zoom_MinError->SetVisibility(ESlateVisibility::Hidden);
		Zoom_MaxError->SetVisibility(ESlateVisibility::Hidden);
		Zoom->SetText(FText::AsNumber(IntZoom));

	}

}

void UJSH_Record_UI::Reset_Zoom_Update()
{
	Zoom->SetText(FText::FromString(TEXT("Default")));
}

void UJSH_Record_UI::Sensitivity_Update(float Mouseupdate)
{
	if (Mouseupdate == 0.7f)
	{
		Sensitivity->SetText(FText::FromString(TEXT("Default")));
	}
	else if (Mouseupdate >= 1.0f)
	{
		Sensitivity->SetText(FText::FromString(TEXT("Max")));
		Sensitivity_MaxError->SetVisibility(ESlateVisibility::Visible);
	}
	else if (Mouseupdate <= 0.06f)
	{
		Sensitivity->SetText(FText::FromString(TEXT("Min")));
		Sensitivity_MinError->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		Sensitivity_MinError->SetVisibility(ESlateVisibility::Hidden);
		Sensitivity_MaxError->SetVisibility(ESlateVisibility::Hidden);
		// 소수점 둘째 자리까지 항상 표시되도록
		FString formattedValue = FString::Printf(TEXT("%.2f"), Mouseupdate);
		Sensitivity->SetText(FText::FromString(formattedValue));
	}
}

void UJSH_Record_UI::CameraUI_Rotate_Update(float Cameraupdate)
{
	if (Cameraupdate == 0.0f)
	{
		CameraUI_Rotate->SetText(FText::FromString(TEXT("Default")));
	}
	else
	{
		int32 intCameraupdate = FMath::RoundToInt(Cameraupdate);
		CameraUI_Rotate->SetText(FText::AsNumber(intCameraupdate));
	}
}
