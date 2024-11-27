// Fill out your copyright notice in the Description page of Project Settings.


#include "../JSH/Widget/JSH_VideoStart.h"

#include "Components/Image.h"

class AJSH_BetaPlayer;

void UJSH_VideoStart::NativeConstruct()
{
	Super::NativeConstruct();


	Guide02->SetVisibility(ESlateVisibility::Hidden);
	Button_Back->SetVisibility(ESlateVisibility::Hidden);
	Button_Next->SetVisibility(ESlateVisibility::Visible);
	
	OverlapImage->SetVisibility(ESlateVisibility::Hidden);
	
	if(Button_Next) Button_Next->OnClicked.AddDynamic(this, &UJSH_VideoStart::OnButton_Next);
	if(Button_Back) Button_Back->OnClicked.AddDynamic(this, &UJSH_VideoStart::OnButton_Back);
	if(Button_Exe) Button_Exe->OnClicked.AddDynamic(this, &UJSH_VideoStart::OnButton_Exe);
}


void UJSH_VideoStart::Saved_BetaPlayer(AJSH_BetaPlayer* pl)
{
	UE_LOG(LogTemp, Warning, TEXT("buton4"));
	OriginPlayer = pl;
}


void UJSH_VideoStart::OnButton_Next()
{
	UE_LOG(LogTemp, Warning, TEXT("buton1"));
	Guide01->SetVisibility(ESlateVisibility::Hidden);
	Button_Next->SetVisibility(ESlateVisibility::Hidden);
	
	Guide02->SetVisibility(ESlateVisibility::Visible);
	Button_Back->SetVisibility(ESlateVisibility::Visible);
}

void UJSH_VideoStart::OnButton_Back()
{
	UE_LOG(LogTemp, Warning, TEXT("buton2"));
	Guide01->SetVisibility(ESlateVisibility::Visible);
	Button_Next->SetVisibility(ESlateVisibility::Visible);
	
	Guide02->SetVisibility(ESlateVisibility::Hidden);
	Button_Back->SetVisibility(ESlateVisibility::Hidden);
}

void UJSH_VideoStart::OnButton_Exe()
{
	Guide01->SetVisibility(ESlateVisibility::Hidden);
	Guide02->SetVisibility(ESlateVisibility::Hidden);
	Button_Next->SetVisibility(ESlateVisibility::Hidden);
	Button_Back->SetVisibility(ESlateVisibility::Hidden);
	Button_Exe->SetVisibility(ESlateVisibility::Hidden);
	OriginPlayer->MouseCursor_Off();
	
	// UE_LOG(LogTemp, Warning, TEXT("buton3"));
	
}

