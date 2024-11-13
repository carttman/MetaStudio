// Fill out your copyright notice in the Description page of Project Settings.


#include "JYS/SpaceshipAnimInstance.h"
#include "JYS/SpaceshipPawn.h"

void USpaceshipAnimInstance::NativeBeginPlay()
{
	Me = Cast<ASpaceshipPawn>(TryGetPawnOwner());
	check(Me);
}

void USpaceshipAnimInstance::AnimNotify_LegAnimEnded()
{
	UE_LOG(LogTemp, Warning, TEXT("AnimNotify_LegAnimEnded"))
	Me->bCantMove = false;
}

void USpaceshipAnimInstance::AnimNotify_LegUpLoopNotify()
{
	
	UE_LOG(LogTemp, Warning, TEXT("AnimNotify_LegUpLoopNotify"))
	Montage_SetPlayRate(legUpMontage, 0);
}

void USpaceshipAnimInstance::Play_LegUpMontage()
{
	Montage_SetPlayRate(legUpMontage, -1);
	Montage_Play(legUpMontage);
	Montage_JumpToSection(TEXT("LegUpStartSection"));
}

void USpaceshipAnimInstance::Play_LegDownMontage()
{
	Montage_Play(legDownMontage);
}

void USpaceshipAnimInstance::SetLegUpMontagePlayRate()
{
	Montage_SetPlayRate(legUpMontage, -1);
}

void USpaceshipAnimInstance::Play_OpenDoor()
{
	Montage_Play(openDoorMontage);
}

void USpaceshipAnimInstance::SetCloseDoorMontagePlayRate()
{
	Montage_SetPlayRate(closeDoorMontage, -1);
	Montage_JumpToSection(TEXT("closeDoorStartSection"));
}

