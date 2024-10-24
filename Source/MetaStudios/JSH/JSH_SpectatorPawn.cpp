// Fill out your copyright notice in the Description page of Project Settings.


#include "JSH_SpectatorPawn.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/Controller.h"
#include "JSH_Player.h"
#include "JSH_PlayerController.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "Components/SceneComponent.h"
#include "GameFramework/MovementComponent.h"
#include "GameFramework/SpectatorPawnMovement.h"
#include "Kismet/GameplayStatics.h"
#include "Tests/AutomationCommon.h"


void AJSH_SpectatorPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);


	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		EnhancedInputComponent->BindAction(IA_BackMainPlayer, ETriggerEvent::Started, this, & AJSH_SpectatorPawn::BackPlayer);
		EnhancedInputComponent->BindAction(IA_EditMode, ETriggerEvent::Triggered, this, & AJSH_SpectatorPawn::EditModeOFF);
		EnhancedInputComponent->BindAction(IA_EditMode, ETriggerEvent::Ongoing, this, & AJSH_SpectatorPawn::EditModeOFF);
		EnhancedInputComponent->BindAction(IA_EditMode, ETriggerEvent::Completed, this, & AJSH_SpectatorPawn::EditModeON);
	}
	else
	{
		//UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AJSH_SpectatorPawn::BeginPlay()
{
	Super::BeginPlay();

	
	SpectatorPawnMovementComponent = Cast<USpectatorPawnMovement>(GetMovementComponent());
	OriginController = Cast<APlayerController>(GetController());
	
	EnableEdit();
}




#pragma region MainPlayer <-> SpectatorPawn

// (F) 에디터 모드에서 다시 플레이어 모드로 돌아가는 함수 
void AJSH_SpectatorPawn::BackPlayer()
{
	if (HasAuthority())
	{
		NetMulti_BackPlayer();
	}
}

void AJSH_SpectatorPawn::NetMulti_BackPlayer_Implementation()
{
	// 1. Get Player Controller (인덱스가 0인 플레이어)
	//APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	//APlayerController* PlayerController = Cast<APlayerController>(GetWorld()->GetFirstPlayerController());
	
	AJSH_PlayerController* PlayerController = Cast<AJSH_PlayerController>(GetWorld()->GetFirstPlayerController());
    
	if (PlayerController)
	{
		PlayerController->Possess(PlayerController->OriginPlayer);

		UE_LOG(LogTemp, Error, TEXT("22222222"));
		PlayerController->OriginPlayer->Visible_On_OFF();
		// 4. 자기 자신(SpectatorPawn) Destory
		// this->Destroy();
	}

	DisableEdit();
}


#pragma endregion


#pragma region EditMode


void AJSH_SpectatorPawn::EditModeON()
{
	if (HasAuthority())
	{
		EnableEdit();
	}
}

void AJSH_SpectatorPawn::EditModeOFF()
{
	if (HasAuthority())
	{
		DisableEdit();
	}
}


void AJSH_SpectatorPawn::EnableEdit()
{
	if (SpectatorPawnMovementComponent)
	{
		SpectatorPawnMovementComponent->SetComponentTickEnabled(false);
	}

	if (OriginController)
	{
		// 마우스 커서 보이게 설정
		OriginController->bShowMouseCursor = true;
		OriginController->bEnableClickEvents = true;
		OriginController->bEnableMouseOverEvents = true;

		// 마우스를 UI와 게임에서 사용할 수 있도록 설정
		OriginController->SetInputMode(FInputModeGameAndUI());
		GEngine->GameViewport->SetMouseLockMode(EMouseLockMode::LockAlways);
	}
}




void AJSH_SpectatorPawn::DisableEdit()
{
	if (SpectatorPawnMovementComponent)
	{
		SpectatorPawnMovementComponent->SetComponentTickEnabled(true);
	}

	if (OriginController)
	{
		OriginController->SetIgnoreLookInput(false);
		
		OriginController->bShowMouseCursor = false;
		OriginController->bEnableClickEvents = false;
		OriginController->bEnableMouseOverEvents = false;
		OriginController->SetInputMode(FInputModeGameOnly());
	}
}

#pragma endregion






