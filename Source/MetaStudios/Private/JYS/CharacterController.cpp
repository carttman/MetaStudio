// Fill out your copyright notice in the Description page of Project Settings.


#include "JYS/CharacterController.h"
#include "JYS/MetaStudiosCharacter.h"

void ACharacterController::SaveCharacter()
{
	characterPlayer = Cast<AMetaStudiosCharacter>(GetPawn());
}
