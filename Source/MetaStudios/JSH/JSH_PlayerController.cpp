// Fill out your copyright notice in the Description page of Project Settings.


#include "JSH_PlayerController.h"



void AJSH_PlayerController::SaveOriginCharacter()
{
	// OriginPlayer->SetOwner(GetOwner());

	// OriginPlayer = Cast<AJSH_Player>(GetOwner());
	
	// OriginPlayer = Cast<AJSH_Player>(GetOwner());

	OriginPlayer = Cast<AJSH_Player>(GetPawn());

}
