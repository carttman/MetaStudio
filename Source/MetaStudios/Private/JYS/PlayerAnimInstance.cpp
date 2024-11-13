// Fill out your copyright notice in the Description page of Project Settings.


#include "JYS/PlayerAnimInstance.h"
#include "JYS/MetaStudiosCharacter.h"

void UPlayerAnimInstance::NativeBeginPlay()
{
	Me = Cast<AMetaStudiosCharacter>(TryGetPawnOwner());
	check(Me);
}

void UPlayerAnimInstance::Play_pickUpMontage()
{
	Montage_Play(pickUpMontage);
}

