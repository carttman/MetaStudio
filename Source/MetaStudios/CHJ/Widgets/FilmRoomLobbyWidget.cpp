// Fill out your copyright notice in the Description page of Project Settings.


#include "../CHJ/Widgets/FilmRoomLobbyWidget.h"

#include "FIlmSessionSlotWidget.h"
#include "Components/Button.h"
#include "Components/EditableText.h"
#include "Components/ScrollBox.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "MetaStudios/CHJ/MainGameInstance.h"

void UFilmRoomLobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 게임인스턴스를 가져와서
	auto* gi = Cast<UMainGameInstance>(GetWorld()->GetGameInstance());
	// OnSearchSignatureCompleteDelegate에 AddSessionSlotWidget을 연결하고싶다.
	gi->OnSearchSignatureCompleteDelegate.AddDynamic(this , &UFilmRoomLobbyWidget::AddSessionSlotWidget);
	gi->OnFindSignatureCompleteDelegate.AddDynamic(this , &UFilmRoomLobbyWidget::SetFindActive);


	MENU_Button_GoCreateRoom->OnClicked.AddDynamic(this , &UFilmRoomLobbyWidget::MENU_OnClickGoCreateRoom);
	MENU_Button_GoFindSessions->OnClicked.AddDynamic(this , &UFilmRoomLobbyWidget::MENU_OnClickGoFindSessions);

	CR_Button_GoMenu->OnClicked.AddDynamic(this , &UFilmRoomLobbyWidget::OnClickGoMenu);
	FS_Button_GoMenu->OnClicked.AddDynamic(this , &UFilmRoomLobbyWidget::OnClickGoMenu);

	CR_Button_CreateRoom->OnClicked.AddDynamic(this , &UFilmRoomLobbyWidget::CR_OnClickCreateRoom);
	CR_Slider_PlayerCount->OnValueChanged.AddDynamic(this , &UFilmRoomLobbyWidget::CR_OnChangeSliderPlayerCount);

	FS_Button_FindSessions->OnClicked.AddDynamic(this , &UFilmRoomLobbyWidget::FS_OnClickFindSessions);

	CR_Slider_PlayerCount->SetValue(2);
	SetFindActive(false);
}

void UFilmRoomLobbyWidget::OnClickGoMenu()
{
	LobbyWidgetSwitcher->SetActiveWidgetIndex(0);
}

void UFilmRoomLobbyWidget::MENU_OnClickGoCreateRoom()
{
	// MENU_Edit_SessionName의 내용을 UNetTPSGameInstance의 MySessionName 에 반영하고싶다.
	auto* gi = Cast<UMainGameInstance>(GetWorld()->GetGameInstance());
	FString newSessionName = MENU_Edit_SessionName->GetText().ToString();
	if ( gi && false == newSessionName.IsEmpty() )
	{
		gi->MySessionName = MENU_Edit_SessionName->GetText().ToString();
	}
	LobbyWidgetSwitcher->SetActiveWidgetIndex(1);
}

void UFilmRoomLobbyWidget::MENU_OnClickGoFindSessions()
{
	// MENU_Edit_SessionName의 내용을 UNetTPSGameInstance의 MySessionName 에 반영하고싶다.
	auto* gi = Cast<UMainGameInstance>(GetWorld()->GetGameInstance());
	FString newSessionName = MENU_Edit_SessionName->GetText().ToString();
	if ( gi && false == newSessionName.IsEmpty() )
	{
		gi->MySessionName = MENU_Edit_SessionName->GetText().ToString();
	}
	LobbyWidgetSwitcher->SetActiveWidgetIndex(2);

	FS_OnClickFindSessions();
}

void UFilmRoomLobbyWidget::CR_OnClickCreateRoom()
{
	auto* gi = Cast<UMainGameInstance>(GetWorld()->GetGameInstance());
	FString roomName = CR_Edit_RoomName->GetText().ToString();

	// roomName이 기재되지 않거나 공백이라면 방생성을 하지 않고싶다.
	roomName = roomName.TrimStartAndEnd();
	if ( roomName.IsEmpty() )
	{
		return;
	}

	int32 count = (int32)CR_Slider_PlayerCount->GetValue();
	gi->CreateMySession(roomName , count, 1);
}

void UFilmRoomLobbyWidget::CR_OnChangeSliderPlayerCount(float value)
{
	// 슬라이더의 값이 변경되면 CR_Text_PlayerCount에 값을 반영하고 싶다.
	CR_Text_PlayerCount->SetText(FText::AsNumber(value));
}

void UFilmRoomLobbyWidget::FS_OnClickFindSessions()
{
	// 기존의 방 목록을 삭제하고
	FS_ScrollBox->ClearChildren();

	// 방찾기를 요청하고싶다.
	auto* gi = Cast<UMainGameInstance>(GetWorld()->GetGameInstance());
	if ( gi )
	{
		gi->FindOtherSessions();
	}
}

void UFilmRoomLobbyWidget::AddSessionSlotWidget(const struct FRoomInfo& info)
{
	// 슬롯을 생성해서 
	auto* slot = CreateWidget<UFIlmSessionSlotWidget>(this , SessionSlotWidgetFactory);

	slot->UpdateInfo(info);
	// FS_ScrollBox에 추가 하고싶다.

	FS_ScrollBox->AddChild(slot);
}

void UFilmRoomLobbyWidget::SetFindActive(bool value)
{
	// 찾기를 시도하면 Find텍스트를 보이게, 버튼 비활성 하고싶다.
	// 찾기가 끝나면 Find텍스트 안보이게, 버튼 활성 하고싶다.
	if ( value )
	{
		// 찾기 시도
		FS_Text_Finding->SetVisibility(ESlateVisibility::Visible);
		FS_Button_FindSessions->SetIsEnabled(false);
	}
	else
	{
		// 찾기 끝
		FS_Text_Finding->SetVisibility(ESlateVisibility::Hidden);
		FS_Button_FindSessions->SetIsEnabled(true);
	}
}
