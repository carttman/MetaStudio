// Fill out your copyright notice in the Description page of Project Settings.


#include "../../JSH/Widget/JSH_Editor_W01.h"
#include "DrawDebugHelpers.h"
#include "Framework/Application/SlateApplication.h"
#include "Engine/World.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "MetaStudios/JSH/JSH_Editor_SpawnActor.h"


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

		UE_LOG(LogTemp, Error, TEXT("WWWClick"));

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
	
	UE_LOG(LogTemp, Error, TEXT("WWW222Click"));
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


// void UJSH_Editor_W01::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
// {
//     Super::NativeOnDragCancelled(InDragDropEvent, InOperation);
//
// 	UE_LOG(LogTemp, Error, TEXT("11 cancelled"));
// 	
//     
//     FVector2D MousePosition;
// 	
//     if (GetWorld()->GetGameViewport()->GetMousePosition(MousePosition))
//     {
//     	UE_LOG(LogTemp, Error, TEXT("22"));
//         FVector WorldPosition;
//         FVector WorldDirection;
//
//         
//         PlayerController = GetWorld()->GetFirstPlayerController();
//         if (PlayerController)
//         {
//         	UE_LOG(LogTemp, Error, TEXT("333"));
//             PlayerController->DeprojectScreenPositionToWorld(MousePosition.X, MousePosition.Y, WorldPosition, WorldDirection);
//
//            
//             Start = WorldPosition;
//             End = Start + (WorldDirection * 2000.0f);
//
//             
//             FHitResult HitResult;
//             FCollisionQueryParams Params;
//             Params.AddIgnoredActor(PlayerController->GetPawn());
//
//             bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);
//
//         	
//         	if (bHit)
//         	{
//         		UE_LOG(LogTemp, Error, TEXT("bhit"));
//         		FActorSpawnParameters SpawnParams;
//         		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
// 	
//         		AGameModeBase* tt = Cast<AGameModeBase>(GetWorld()->GetAuthGameMode());
// 			
//         		UClass* SpectatorClass = StaticLoadClass(AActor::StaticClass(), nullptr, TEXT("/Game/JSH/BP/BP_Asset1.BP_Asset1_C"));
// 			
//         		AActor* SpectatorActor = GetWorld()->SpawnActorDeferred<AActor>(SpectatorClass, HitResult.GetActor()->GetActorTransform());
//
// 			
//         		if (SpectatorActor)
//         		{
//         			UE_LOG(LogTemp, Error, TEXT("spectator"));
//         			APawn* SpectatorPawn = Cast<APawn>(SpectatorActor);
//         			if (SpectatorPawn)
//         			{
//         				UGameplayStatics::FinishSpawningActor(SpectatorActor, HitResult.GetActor()->GetActorTransform());
//         			}
//         		}
//         	}
//
//         }
//     }
// }

