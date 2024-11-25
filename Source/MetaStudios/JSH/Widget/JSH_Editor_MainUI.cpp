// Fill out your copyright notice in the Description page of Project Settings.


#include "../../JSH/Widget/JSH_Editor_MainUI.h"

#include "Components/Image.h"

void UJSH_Editor_MainUI::NativeConstruct()
{
	Super::NativeConstruct();

	SelectMode2->SetVisibility(ESlateVisibility::Hidden);
	TranslateMode2->SetVisibility(ESlateVisibility::Hidden);
	RotateMode2->SetVisibility(ESlateVisibility::Hidden);
	ScaleMode2->SetVisibility(ESlateVisibility::Hidden);
}
