// Fill out your copyright notice in the Description page of Project Settings.


#include "../CHJ/CinemaGameMode.h"

#include "JYS/MetaStudiosCharacter.h"
#include "MetaStudios/JSH/JSH_BetaPlayer.h"

APawn* ACinemaGameMode::SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot)
{
	UE_LOG(LogTemp, Warning, TEXT("this Controller is : %s"), *NewPlayer->GetName());
	return Super::SpawnDefaultPawnFor_Implementation(NewPlayer, StartSpot);
	
	// if(NewPlayer->IsLocalController())
	// {
	// 	return GetWorld()->SpawnActor<AJSH_BetaPlayer>(ServerPlayer, StartSpot->GetActorLocation(), StartSpot->GetActorRotation());
	// }
	// else
	// {
	// 	return GetWorld()->SpawnActor<AMetaStudiosCharacter>(ClientPlayer, StartSpot->GetActorLocation(), StartSpot->GetActorRotation());
	// }
}
