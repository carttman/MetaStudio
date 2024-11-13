// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyWidget.generated.h"

/**
 * 
 */
UCLASS()
class METASTUDIOS_API ULobbyWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	UPROPERTY(meta=(BindWidget))
	class UWidgetSwitcher* LobbyWidgetSwitcher;

	UFUNCTION()
	void OnClickGoMenu();

	// Menu ===============================================
	UPROPERTY(meta=(BindWidget))
	class UButton* MENU_Button_GoCreateRoom;

	UFUNCTION()
	void MENU_OnClickGoCreateRoom();

	UFUNCTION()
	void MENU_OnClickGoFindSessions();
	// CreateRoom ===============================================

	UPROPERTY(meta=(BindWidget))
	class UButton* CR_Button_CreateRoom;

	UPROPERTY(meta=(BindWidget))
	class UEditableText* CR_Edit_RoomName;

	
	
	UPROPERTY(meta=(BindWidget))
	class UButton* CR_Button_GoMenu;

	UFUNCTION()
	void CR_OnClickCreateRoom();

	// FindSessions ===============================================
	UPROPERTY(meta=(BindWidget))
	class UScrollBox* FS_ScrollBox;

	UPROPERTY(meta=(BindWidget))
	class UButton* FS_Button_FindSessions;

	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	class UButton* FS_Button_GoMenu;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class USessionSlotWidget> SessionSlotWidgetFactory;

	UFUNCTION()
	void FS_OnClickFindSessions();

	UFUNCTION()
	void AddSessionSlotWidget(const struct FRoomInfo& info, int32 RoomType);

	// 찾기를 시도하면 Find텍스트를 보이게, 버튼 비활성 하고싶다.
	// 찾기가 끝나면 Find텍스트 안보이게, 버튼 활성 하고싶다.
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* FS_Text_Finding;

	UFUNCTION()
	void SetFindActive(bool value);

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	class UUniformGridPanel* UGP_Grid;
	UPROPERTY()
	TArray<TObjectPtr<USessionSlotWidget>> Slots;

	UPROPERTY(meta=(BindWidget))
	class UEditableText* TB_TEXT_SearchSession;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* T_MyNickName;

	UPROPERTY(meta=(BindWidget))
	class UOverlay* O_CreateRoom;
	
	UFUNCTION()
	void ExitMenu();
	
	UPROPERTY(meta=(BindWidget))
	class UComboBoxString* CBS_PlayerCountCombo;

	UPROPERTY(meta=(BindWidget))
	class UComboBoxString* CBS_PrivateCombo;

	TArray<FString> MaxPlayerCount;
	TArray<FString> FilmRoomValue;
	
	void GetPlayerNickName();
	
	UPROPERTY(meta=(BindWidget))
	class UCircularThrobber* Trobber_FindSession;
};
