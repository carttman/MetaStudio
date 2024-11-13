// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FilmRoomLobbyWidget.generated.h"

/**
 * 
 */
UCLASS()
class METASTUDIOS_API UFilmRoomLobbyWidget : public UUserWidget
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UButton* FS_Button_GoMenu;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<class UFIlmSessionSlotWidget> SessionSlotWidgetFactory;

	UFUNCTION()
	void FS_OnClickFindSessions();
	
	UFUNCTION(BlueprintCallable)
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
	TArray<TObjectPtr<UFIlmSessionSlotWidget>> Slots;

	UPROPERTY(meta=(BindWidget))
	class UEditableText* TB_TEXT_SearchSession;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* T_MyNickName;

	void GetPlayerNickName();

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
	
	void SetFilmRoomCombo();

	UPROPERTY(meta=(BindWidget))
	class UCircularThrobber* Trobber_FindSession;
	
};
