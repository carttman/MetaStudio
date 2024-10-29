// Fill out your copyright notice in the Description page of Project Settings.


#include "../../CHJ/Widgets/LobbyWidget.h"

#include "OnlineSubsystem.h"
#include "Components/Button.h"
#include "Components/EditableText.h"
#include "Components/ScrollBox.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "MetaStudios/CHJ/MainGameInstance.h"
#include "SessionSlotWidget.h"
#include "Components/ComboBoxString.h"
#include "Components/Overlay.h"
#include "Components/UniformGridPanel.h"
#include "Interfaces/OnlineIdentityInterface.h"

void ULobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();

	auto* gi = Cast<UMainGameInstance>(GetWorld()->GetGameInstance());
	// 딜리게이트 바인딩
	gi->OnSearchSignatureCompleteDelegate.AddDynamic(this , &ULobbyWidget::AddSessionSlotWidget);
	gi->OnFindSignatureCompleteDelegate.AddDynamic(this , &ULobbyWidget::SetFindActive);
	
	MENU_Button_GoCreateRoom->OnClicked.AddDynamic(this , &ULobbyWidget::MENU_OnClickGoCreateRoom);

	CR_Button_GoMenu->OnClicked.AddDynamic(this , &ULobbyWidget::OnClickGoMenu);
	FS_Button_GoMenu->OnClicked.AddDynamic(this , &ULobbyWidget::ExitMenu);

	CR_Button_CreateRoom->OnClicked.AddDynamic(this , &ULobbyWidget::CR_OnClickCreateRoom);

	FS_Button_FindSessions->OnClicked.AddDynamic(this , &ULobbyWidget::FS_OnClickFindSessions);
	
	SetFindActive(false);
	GetPlayerNickName();
	FS_OnClickFindSessions();

	for(int32 i=2; i < 40; i++)
	{
		CBS_PlayerCountCombo->AddOption(FString::FromInt(i));
	}
}

void ULobbyWidget::OnClickGoMenu()
{
	//LobbyWidgetSwitcher->SetActiveWidgetIndex(0);
	O_CreateRoom->SetVisibility(ESlateVisibility::Hidden);

}

void ULobbyWidget::MENU_OnClickGoCreateRoom()
{
	// // MENU_Edit_SessionName의 내용을 UNetTPSGameInstance의 MySessionName 에 반영하고싶다.
	// auto* gi = Cast<UMainGameInstance>(GetWorld()->GetGameInstance());
	// FString newSessionName = MENU_Edit_SessionName->GetText().ToString();
	// if ( gi && false == newSessionName.IsEmpty() )
	// {
	// 	gi->MySessionName = MENU_Edit_SwessionName->GetText().ToString();
	// }
	//LobbyWidgetSwitcher->SetActiveWidgetIndex(1);
	O_CreateRoom->SetVisibility(ESlateVisibility::Visible);

}

void ULobbyWidget::MENU_OnClickGoFindSessions()
{
	// // MENU_Edit_SessionName의 내용을 UNetTPSGameInstance의 MySessionName 에 반영하고싶다.
	// auto* gi = Cast<UMainGameInstance>(GetWorld()->GetGameInstance());
	// FString newSessionName = MENU_Edit_SessionName->GetText().ToString();
	// if ( gi && false == newSessionName.IsEmpty() )
	// {
	// 	gi->MySessionName = MENU_Edit_SessionName->GetText().ToString();
	// }
	LobbyWidgetSwitcher->SetActiveWidgetIndex(2);

	FS_OnClickFindSessions();
}

void ULobbyWidget::CR_OnClickCreateRoom()
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
	UE_LOG(LogTemp, Warning, TEXT("222222222222 : %d"), count);
	gi->CreateMySession(roomName , count, 0);
}



void ULobbyWidget::FS_OnClickFindSessions()
{
	UGP_Grid->ClearChildren();
	// 방찾기를 요청하고싶다.
	auto* gi = Cast<UMainGameInstance>(GetWorld()->GetGameInstance());
	if ( gi )
	{
		gi->FindOtherSessions();
	}
}

void ULobbyWidget::AddSessionSlotWidget(const struct FRoomInfo& info, int32 RoomType)
{
	//RoomType이 0인것만 탐색한다.
	if (RoomType == 1) return;
	//검색칸에 내용이 있고 방이름에 검색칸 내용이 포함 되어 있다면
	if(!TB_TEXT_SearchSession->GetText().IsEmpty() && info.roomName.Contains(TB_TEXT_SearchSession->GetText().ToString()))
	{
		int32 index = UGP_Grid->GetChildrenCount();
		int32 X = index % 2;
		int32 Y = index / 2;
		auto* slot = CreateWidget<USessionSlotWidget>(this , SessionSlotWidgetFactory);
		slot->UpdateInfo(info);
		
		UGP_Grid->AddChildToUniformGrid(slot, Y, X);
	}
	else if(TB_TEXT_SearchSession->GetText().IsEmpty())
	{
		int32 index = UGP_Grid->GetChildrenCount();
		int32 X = index % 2;
		int32 Y = index / 2;
		auto* slot = CreateWidget<USessionSlotWidget>(this , SessionSlotWidgetFactory);
		
		slot->UpdateInfo(info);
		
		UGP_Grid->AddChildToUniformGrid(slot, Y, X);
	}
}

void ULobbyWidget::SetFindActive(bool value)
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

void ULobbyWidget::ExitMenu()
{
}

void ULobbyWidget::GetPlayerNickName()
{//스팀 닉네임을 가져온다
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
