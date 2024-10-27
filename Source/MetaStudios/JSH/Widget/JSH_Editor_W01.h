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

private:
	// FGeometry를 직접 저장하는 대신 필요한 정보만 저장
	FVector2D LastMousePosition;
	bool bIsDragging;
};
