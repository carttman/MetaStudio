// Fill out your copyright notice in the Description page of Project Settings.


#include "../CHJ/FilmRoomMainGameMode.h"

void AFilmRoomMainGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	//	Player = Cast<APlayerController>(GetWorld()->GetFirstPlayerController()->GetPawn());
}
