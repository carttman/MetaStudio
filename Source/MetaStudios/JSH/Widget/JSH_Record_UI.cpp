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

	Rec->SetVisibility(ESlateVisibility::Hidden);
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
	Rec->SetVisibility(ESlateVisibility::Visible);

	BackGround->SetVisibility(ESlateVisibility::Hidden);
	BackGround2->SetVisibility(ESlateVisibility::Visible);
	StartRecord = true;
}

void UJSH_Record_UI::StopRecrod_Anim()
{
	Rec->SetVisibility(ESlateVisibility::Hidden);


	BackGround->SetVisibility(ESlateVisibility::Visible);
	BackGround2->SetVisibility(ESlateVisibility::Hidden);
	StartRecord = false;
}

void UJSH_Record_UI::Speed_Update(FText NewText)
{
	Speed->SetText(NewText);
}
