// Fill out your copyright notice in the Description page of Project Settings.


#include "../../CHJ/Widgets/SessionSlotWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "MetaStudios/CHJ/MainGameInstance.h"

void USessionSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	Button_Join->OnClicked.AddDynamic(this , &USessionSlotWidget::OnClickJoin);
}

void USessionSlotWidget::OnClickJoin()
{
	// SessionSearchIndex
	auto* gi = Cast<UMainGameInstance>(GetWorld()->GetGameInstance());
	if ( gi )
	{
		gi->JoinMySession(SessionSearchIndex);
	}
}

void USessionSlotWidget::UpdateInfo(const struct FRoomInfo& info)
{
	Text_RoomName->SetText(FText::FromString(info.roomName));
	FString newRoomName = info.hostName.Left(8);
	Text_HostName->SetText(FText::FromString(newRoomName));
	FString count = FString::Printf(TEXT("%d / %d") , info.currentPlayerCount, info.maxPlayerCount);
	Text_PlayerCount->SetText(FText::FromString(count));
	Text_PingMS->SetText(FText::AsNumber(info.pingMS));

	SessionSearchIndex = info.index;
}
