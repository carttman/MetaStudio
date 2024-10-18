// Fill out your copyright notice in the Description page of Project Settings.


#include "JSH_PlayerController.h"



void AJSH_PlayerController::SaveOriginCharacter()
{
	if (HasAuthority())
	{
		NetMulti_SaveOriginCharacter();
	}
}
void AJSH_PlayerController::NetMulti_SaveOriginCharacter_Implementation()
{
	OriginPlayer = Cast<AJSH_Player>(GetPawn());
}



void AJSH_PlayerController::PlayerViewOnOff()
{
	// UE_LOG(LogTemp, Error, TEXT("ccccccc"));
	// OriginPlayer->Visible_On_OFF();
}
