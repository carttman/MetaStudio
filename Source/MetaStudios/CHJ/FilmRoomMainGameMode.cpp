// Fill out your copyright notice in the Description page of Project Settings.


#include "../CHJ/FilmRoomMainGameMode.h"

#include "MainGameInstance.h"
#include "JYS/MetaStudiosCharacter.h"
#include "MetaStudios/JSH/JSH_Player.h"

APawn* AFilmRoomMainGameMode::SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot)
{
	UE_LOG(LogTemp, Warning, TEXT("this Controller is : %s"), *NewPlayer->GetName());

	auto gi = Cast<UMainGameInstance>(GetWorld()->GetGameInstance());

	if(HasAuthority())
	{
	if(NewPlayer->IsLocalController())
	{
		if(gi->PlayerType == TEXT("감독"))
		{
			return GetWorld()->SpawnActor<AJSH_Player>(ServerPlayer, StartSpot->GetActorLocation(), StartSpot->GetActorRotation());
		}
		else if(gi->PlayerType == TEXT("플레이어"))
		{
			return GetWorld()->SpawnActor<AMetaStudiosCharacter>(ClientPlayer, StartSpot->GetActorLocation(), StartSpot->GetActorRotation());
		}
	}
	}
	
	if(NewPlayer->IsLocalController())
	{
		return GetWorld()->SpawnActor<AJSH_Player>(ServerPlayer, StartSpot->GetActorLocation(), StartSpot->GetActorRotation());
	}
	else
	{
		return GetWorld()->SpawnActor<AMetaStudiosCharacter>(ClientPlayer, StartSpot->GetActorLocation(), StartSpot->GetActorRotation());
	}
	//return nullptr;
}


