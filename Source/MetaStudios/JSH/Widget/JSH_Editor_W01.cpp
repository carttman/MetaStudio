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

void UJSH_Editor_W01::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,
	UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	PlayerMainUI = CreateWidget<UUserWidget>(GetWorld()->GetFirstPlayerController(), UI_Editor_WBP01);
	
	
	if (PlayerMainUI)
	{
		// Drag Drop Operation 생성
		UDragDropOperation* DragDropOp = NewObject<UDragDropOperation>(this, UDragDropOperation::StaticClass());

		if (DragDropOp)
		{
			// Payload 및 Default Drag Visual 설정
			DragDropOp->Payload = PlayerMainUI; // Payload에 필요한 데이터를 설정할 수 있습니다.
			DragDropOp->DefaultDragVisual = PlayerMainUI; // 드래그 시 보여질 비주얼 설정
			DragDropOp->Pivot = EDragPivot::CenterCenter; // 드래그 핏 설정 (CenterCenter)

			// 생성한 DragDropOperation을 OutOperation에 할당
			OutOperation = DragDropOp;
		}
	}
	
}
