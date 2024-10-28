#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/DragDropOperation.h"
#include "Components/Widget.h"
#include "JSH_Editor_W01.generated.h"



UCLASS()
class METASTUDIOS_API UJSH_Editor_W01 : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	class UImage* Test_Image;


protected:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	
public:
	// FGeometry를 직접 저장하는 대신 필요한 정보만 저장
	UPROPERTY()
	FVector2D LastMousePosition;
	UPROPERTY()
	bool bIsDragging;


	// UI
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UUserWidget> UI_Editor_WBP01;
	
	UPROPERTY(EditDefaultsOnly)
	class UUserWidget* PlayerMainUI;
};
