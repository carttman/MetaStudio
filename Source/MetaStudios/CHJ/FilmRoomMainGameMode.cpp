// Fill out your copyright notice in the Description page of Project Settings.


#include "../CHJ/FilmRoomMainGameMode.h"

#include "JYS/MetaStudiosCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "MetaStudios/JSH/JSH_Player.h"

APawn* AFilmRoomMainGameMode::SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot)
{
	UE_LOG(LogTemp, Warning, TEXT("this Controller is : %s"), *NewPlayer->GetName());
	
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


