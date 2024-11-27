// Fill out your copyright notice in the Description page of Project Settings.


#include "../../JSH/Widget/JSH_Editor_MainUI.h"

#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"
#include "MetaStudios/JSH/JSH_Player.h"

void UJSH_Editor_MainUI::NativeConstruct()
{
	Super::NativeConstruct();

	SelectMode2->SetVisibility(ESlateVisibility::Hidden);
	TranslateMode2->SetVisibility(ESlateVisibility::Hidden);
	RotateMode2->SetVisibility(ESlateVisibility::Hidden);
	ScaleMode2->SetVisibility(ESlateVisibility::Hidden);

	Window1->SetVisibility(ESlateVisibility::Hidden);
	Window2->SetVisibility(ESlateVisibility::Hidden);
	Window3->SetVisibility(ESlateVisibility::Hidden);
	Window4->SetVisibility(ESlateVisibility::Hidden);
	Window5->SetVisibility(ESlateVisibility::Hidden);
	Image_window2->SetVisibility(ESlateVisibility::Hidden);
	Image_window3->SetVisibility(ESlateVisibility::Hidden);
	WBP_02->SetVisibility(ESlateVisibility::Hidden);
	WBP_04->SetVisibility(ESlateVisibility::Hidden);
	WBP_05->SetVisibility(ESlateVisibility::Hidden);
	WBP_06->SetVisibility(ESlateVisibility::Hidden);


	if(Button_w1) Button_w1->OnClicked.AddDynamic(this, &UJSH_Editor_MainUI::OnButton_w1_Clicked);
	if(Button_w2) Button_w2->OnClicked.AddDynamic(this, &UJSH_Editor_MainUI::OnButton_w2_Clicked);
	if(Button_w3) Button_w3->OnClicked.AddDynamic(this, &UJSH_Editor_MainUI::OnButton_w3_Clicked);
	if(Button_w4) Button_w4->OnClicked.AddDynamic(this, &UJSH_Editor_MainUI::OnButton_w4_Clicked);
	if(Button_w5) Button_w5->OnClicked.AddDynamic(this, &UJSH_Editor_MainUI::OnButton_w5_Clicked);
	if(Button_w5) Button_w5->OnClicked.AddDynamic(this, &UJSH_Editor_MainUI::OnButton_w5_Clicked);


	
	// Preset
	if(Button_Preset_Station) Button_Preset_Station->OnClicked.AddDynamic(this, &UJSH_Editor_MainUI::OnButton_Preset_Station_Clicked);
	Button_Preset_Station->SetVisibility(ESlateVisibility::Hidden);
	if(Button_Preset_Meteor) Button_Preset_Meteor->OnClicked.AddDynamic(this, &UJSH_Editor_MainUI::OnButton_Preset_Meteor_Clicked);
	Button_Preset_Meteor->SetVisibility(ESlateVisibility::Hidden);
	
}


void UJSH_Editor_MainUI::Begin_SavedPlayer(AJSH_Player* temp)
{
	OriginPlayer = temp;
}



void UJSH_Editor_MainUI::OnButton_w1_Clicked()
{
	//Window0->SetVisibility(ESlateVisibility::Hidden);
	Window1->SetVisibility(ESlateVisibility::Visible);
	Window2->SetVisibility(ESlateVisibility::Hidden);
	Window3->SetVisibility(ESlateVisibility::Hidden);
	Window4->SetVisibility(ESlateVisibility::Hidden);
	Window5->SetVisibility(ESlateVisibility::Hidden);
	Image_window2->SetVisibility(ESlateVisibility::Hidden);
	Image_window3->SetVisibility(ESlateVisibility::Hidden);

	WBP_02->SetVisibility(ESlateVisibility::Hidden);
	WBP_04->SetVisibility(ESlateVisibility::Hidden);
	WBP_05->SetVisibility(ESlateVisibility::Hidden);
	WBP_06->SetVisibility(ESlateVisibility::Hidden);
	UE_LOG(LogTemp, Warning, TEXT("button"));
}

void UJSH_Editor_MainUI::OnButton_w2_Clicked()
{
	//Window0->SetVisibility(ESlateVisibility::Hidden);
	Window1->SetVisibility(ESlateVisibility::Hidden);
	Window2->SetVisibility(ESlateVisibility::Visible);
	Window3->SetVisibility(ESlateVisibility::Hidden);
	Window4->SetVisibility(ESlateVisibility::Hidden);
	Window5->SetVisibility(ESlateVisibility::Hidden);
	Image_window2->SetVisibility(ESlateVisibility::Visible);
	Image_window3->SetVisibility(ESlateVisibility::Hidden);
	Button_Preset_Station->SetVisibility(ESlateVisibility::Hidden);
	Button_Preset_Meteor->SetVisibility(ESlateVisibility::Hidden);

	WBP_02->SetVisibility(ESlateVisibility::Visible);
	WBP_04->SetVisibility(ESlateVisibility::Visible);
	WBP_05->SetVisibility(ESlateVisibility::Hidden);
	WBP_06->SetVisibility(ESlateVisibility::Hidden);
}

void UJSH_Editor_MainUI::OnButton_w3_Clicked()
{
	//Window0->SetVisibility(ESlateVisibility::Hidden);
	Window1->SetVisibility(ESlateVisibility::Hidden);
	Window2->SetVisibility(ESlateVisibility::Hidden);
	Window3->SetVisibility(ESlateVisibility::Visible);
	Window4->SetVisibility(ESlateVisibility::Hidden);
	Window5->SetVisibility(ESlateVisibility::Hidden);
	Image_window2->SetVisibility(ESlateVisibility::Hidden);
	Image_window3->SetVisibility(ESlateVisibility::Visible);
	Button_Preset_Station->SetVisibility(ESlateVisibility::Hidden);
	Button_Preset_Meteor->SetVisibility(ESlateVisibility::Hidden);

	WBP_02->SetVisibility(ESlateVisibility::Hidden);
	WBP_04->SetVisibility(ESlateVisibility::Hidden);
	WBP_05->SetVisibility(ESlateVisibility::Visible);
	WBP_06->SetVisibility(ESlateVisibility::Visible);
}

void UJSH_Editor_MainUI::OnButton_w4_Clicked()
{
	//Window0->SetVisibility(ESlateVisibility::Hidden);
	Window1->SetVisibility(ESlateVisibility::Hidden);
	Window2->SetVisibility(ESlateVisibility::Hidden);
	Window3->SetVisibility(ESlateVisibility::Hidden);
	Window4->SetVisibility(ESlateVisibility::Visible);
	Window5->SetVisibility(ESlateVisibility::Hidden);
	Image_window2->SetVisibility(ESlateVisibility::Hidden);
	Image_window3->SetVisibility(ESlateVisibility::Hidden);
	Button_Preset_Station->SetVisibility(ESlateVisibility::Hidden);
	Button_Preset_Meteor->SetVisibility(ESlateVisibility::Hidden);

	WBP_02->SetVisibility(ESlateVisibility::Hidden);
	WBP_04->SetVisibility(ESlateVisibility::Hidden);
	WBP_05->SetVisibility(ESlateVisibility::Hidden);
	WBP_06->SetVisibility(ESlateVisibility::Hidden);
}

void UJSH_Editor_MainUI::OnButton_w5_Clicked()
{
	//Window0->SetVisibility(ESlateVisibility::Hidden);
	Window1->SetVisibility(ESlateVisibility::Hidden);
	Window2->SetVisibility(ESlateVisibility::Hidden);
	Window3->SetVisibility(ESlateVisibility::Hidden);
	Window4->SetVisibility(ESlateVisibility::Hidden);
	Window5->SetVisibility(ESlateVisibility::Visible);
	Image_window2->SetVisibility(ESlateVisibility::Hidden);
	Image_window3->SetVisibility(ESlateVisibility::Hidden);
	Button_Preset_Station->SetVisibility(ESlateVisibility::Visible);
	Button_Preset_Meteor->SetVisibility(ESlateVisibility::Visible);

	WBP_02->SetVisibility(ESlateVisibility::Hidden);
	WBP_04->SetVisibility(ESlateVisibility::Hidden);
	WBP_05->SetVisibility(ESlateVisibility::Hidden);
	WBP_06->SetVisibility(ESlateVisibility::Hidden);
}

void UJSH_Editor_MainUI::OnButton_Preset_Station_Clicked()
{
	UE_LOG(LogTemp, Warning, TEXT("station"));
	OriginPlayer->Preset_On_Off_Station();

}

void UJSH_Editor_MainUI::OnButton_Preset_Meteor_Clicked()
{
	UE_LOG(LogTemp, Warning, TEXT("meteor"));
	OriginPlayer->Preset_On_Off();
}




