// Fill out your copyright notice in the Description page of Project Settings.


#include "../CHJ/Widgets/FilmRoomLobbyWidget.h"

#include "FIlmSessionSlotWidget.h"
#include "OnlineSubsystem.h"
#include "Components/Button.h"
#include "Components/CircularThrobber.h"
#include "Components/ComboBoxString.h"
#include "Components/EditableText.h"
#include "Components/Overlay.h"
#include "Components/ScrollBox.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "Components/UniformGridPanel.h"
#include "Components/WidgetSwitcher.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "MetaStudios/CHJ/MainGameInstance.h"

void UFilmRoomLobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();

	auto* gi = Cast<UMainGameInstance>(GetWorld()->GetGameInstance());
	// 딜리게이트 바인딩
	gi->OnSearchSignatureCompleteDelegate.AddDynamic(this , &UFilmRoomLobbyWidget::AddSessionSlotWidget);
	gi->OnFindSignatureCompleteDelegate.AddDynamic(this , &UFilmRoomLobbyWidget::SetFindActive);
	
	MENU_Button_GoCreateRoom->OnClicked.AddDynamic(this , &UFilmRoomLobbyWidget::MENU_OnClickGoCreateRoom);

	CR_Button_GoMenu->OnClicked.AddDynamic(this , &UFilmRoomLobbyWidget::OnClickGoMenu);
	//FS_Button_GoMenu->OnClicked.AddDynamic(this , &UFilmRoomLobbyWidget::ExitMenu);

	CR_Button_CreateRoom->OnClicked.AddDynamic(this , &UFilmRoomLobbyWidget::CR_OnClickCreateRoom);

	FS_Button_FindSessions->OnClicked.AddDynamic(this , &UFilmRoomLobbyWidget::FS_OnClickFindSessions);

	//CR_Slider_PlayerCount->SetValue(2);
	SetFindActive(false);
	GetPlayerNickName();
	FS_OnClickFindSessions();

	for(int32 i=2; i <= 40; i++)
	{
		CBS_PlayerCountCombo->AddOption(FString::FromInt(i));
	}
	CBS_PlayerType->AddOption(FString(TEXT("감독")));
	CBS_PlayerType->AddOption(FString(TEXT("플레이어")));

	CBS_PlayerCountCombo->SetSelectedIndex(0);
	CBS_PrivateCombo->SetSelectedIndex(0);
	CBS_PlayerType->SetSelectedIndex(0);
}

void UFilmRoomLobbyWidget::OnClickGoMenu()
{
	//LobbyWidgetSwitcher->SetActiveWidgetIndex(0);
	//O_CreateRoom->RemoveFromParent();
	O_CreateRoom->SetVisibility(ESlateVisibility::Hidden);
}

void UFilmRoomLobbyWidget::MENU_OnClickGoCreateRoom()
{
	O_CreateRoom->SetVisibility(ESlateVisibility::Visible);
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
	int32 count = FCString::Atoi(*CBS_PlayerCountCombo->GetSelectedOption());
	gi->PlayerType = CBS_PlayerType->GetSelectedOption();
	
	gi->CreateMySession(roomName , count, 1);
}


void UFilmRoomLobbyWidget::FS_OnClickFindSessions()
{
	// 기존의 방 목록을 삭제한다
	UGP_Grid->ClearChildren();
	// 방찾기를 요청
	auto* gi = Cast<UMainGameInstance>(GetWorld()->GetGameInstance());
	if ( gi )
	{
		gi->FindOtherSessions();
	}
}

void UFilmRoomLobbyWidget::AddSessionSlotWidget(const struct FRoomInfo& info, int32 RoomType)
{
	if (RoomType == 0) return;
	//검색칸에 내용이 있고 방이름에 검색칸 내용이 포함 되어 있다면
	if(!TB_TEXT_SearchSession->GetText().IsEmpty() && info.roomName.Contains(TB_TEXT_SearchSession->GetText().ToString()))
	{
		int32 index = UGP_Grid->GetChildrenCount();
		int32 X = index % 3;
		int32 Y = index / 3;
		auto* slot = CreateWidget<UFIlmSessionSlotWidget>(this , SessionSlotWidgetFactory);
		
		slot->UpdateInfo(info);
		
		UGP_Grid->AddChildToUniformGrid(slot, Y, X);
	}
	else if(TB_TEXT_SearchSession->GetText().IsEmpty())
	{
		int32 index = UGP_Grid->GetChildrenCount();
		int32 X = index % 3;
		int32 Y = index / 3;
		auto* slot = CreateWidget<UFIlmSessionSlotWidget>(this , SessionSlotWidgetFactory);
		
		slot->UpdateInfo(info);
		
		UGP_Grid->AddChildToUniformGrid(slot, Y, X);
	}
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
		Trobber_FindSession->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		// 찾기 끝
		FS_Text_Finding->SetVisibility(ESlateVisibility::Hidden);
		FS_Button_FindSessions->SetIsEnabled(true);
		Trobber_FindSession->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UFilmRoomLobbyWidget::GetPlayerNickName()
{	//스팀 닉네임을 가져온다
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if(Subsystem)
	{
		IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
		if(Identity.IsValid())
		{
			TSharedPtr<const FUniqueNetId> UserId = Identity->GetUniquePlayerId(0);
			if(UserId.IsValid())
			{
				FString Nickname = Identity->GetPlayerNickname(*UserId);
				UE_LOG(LogTemp, Log, TEXT("Steam Nickname: %s"), *Nickname);
				FString N = Nickname.Left(8);
				T_MyNickName->SetText(FText::FromString(*N));
			}
		}
	}
}

void UFilmRoomLobbyWidget::ExitMenu()
{
	//LobbyWidgetSwitcher->SetVisibility(ESlateVisibility::Hidden);
}

void UFilmRoomLobbyWidget::SetFilmRoomCombo()
{
	//CBS_FilmRoomCombo->DefaultOptions

}
