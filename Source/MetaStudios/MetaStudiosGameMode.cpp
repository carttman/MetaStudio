// Copyright Epic Games, Inc. All Rights Reserved.

#include "MetaStudiosGameMode.h"
#include "JYS/MetaStudiosCharacter.h"
#include "UObject/ConstructorHelpers.h"

AMetaStudiosGameMode::AMetaStudiosGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
