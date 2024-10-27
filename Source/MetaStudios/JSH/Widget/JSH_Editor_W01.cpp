// Fill out your copyright notice in the Description page of Project Settings.


#include "../../JSH/Widget/JSH_Editor_W01.h"
#include "Framework/Application/SlateApplication.h"
#include "Components/Image.h"


void UJSH_Editor_W01::NativeConstruct()
{
	Super::NativeConstruct();
	
	
}

FReply UJSH_Editor_W01::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		// 마우스 위치 저장
		LastMousePosition = InMouseEvent.GetScreenSpacePosition();
		bIsDragging = false;

		// 드래그 감지 시작
		FReply Reply = FReply::Handled();
		Reply.DetectDrag(TakeWidget(), EKeys::LeftMouseButton);
		return Reply;
	}

	return FReply::Unhandled();
}