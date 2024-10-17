// Fill out your copyright notice in the Description page of Project Settings.


#include "JSH_SpectatorPawn.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/Controller.h"
#include "JSH_Player.h"
#include "JSH_PlayerController.h"
#include "Utils.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "Components/SceneComponent.h"
#include "GameFramework/MovementComponent.h"
#include "GameFramework/SpectatorPawnMovement.h"
#include "Kismet/GameplayStatics.h"



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
		EnhancedInputComponent->BindAction(IA_EditMode, ETriggerEvent::Ongoing, this, & AJSH_SpectatorPawn::EditModeON);
		EnhancedInputComponent->BindAction(IA_EditMode, ETriggerEvent::Completed, this, & AJSH_SpectatorPawn::EditModeOFF);
	}
	else
	{
		//UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AJSH_SpectatorPawn::BeginPlay()
{
	Super::BeginPlay();

	GEngine->AddOnScreenDebugMessage(-3, 10.0f, FColor::Green, "dsd");
	
	
	SpectatorPawnMovementComponent = Cast<USpectatorPawnMovement>(GetMovementComponent());
	if (SpectatorPawnMovementComponent)
	{
		SpectatorPawnMovementComponent->SetComponentTickEnabled(false);
	}

	UE_LOG(LogTemp, Warning, TEXT("2222222"));

	OriginController = Cast<APlayerController>(GetController());
	OriginController->bShowMouseCursor = true;


	if (OriginController)
	{
		OriginController->SetIgnoreLookInput(true);

		// 마우스 커서를 보이게 설정
		OriginController->bShowMouseCursor = true;

		// 커서를 마우스로 제어할 수 있도록 설정
		OriginController->bEnableClickEvents = true;
		OriginController->bEnableMouseOverEvents = true;

		// 필요하다면 마우스를 UI 전용으로 고정
		// PlayerController->SetInputMode(FInputModeGameAndUI());
	}
}



// (F) 에디터 모드에서 다시 플레이어 모드로 돌아가는 함수 
void AJSH_SpectatorPawn::BackPlayer()
{
	NetMulti_BackPlayer();
}


//
void AJSH_SpectatorPawn::NetMulti_BackPlayer_Implementation()
{
	// 1. Get Player Controller (인덱스가 0인 플레이어)
	//APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	//APlayerController* PlayerController = Cast<APlayerController>(GetWorld()->GetFirstPlayerController());
	
	AJSH_PlayerController* PlayerController = Cast<AJSH_PlayerController>(GetWorld()->GetFirstPlayerController());
    
	if (PlayerController)
	{
		
		PlayerController->Possess(PlayerController->OriginPlayer);

		// 4. 자기 자신(SpectatorPawn) Destory
		this->Destroy();
	}
}

// =====================
#pragma region dlfjdklfj

/////
#pragma endregion


void AJSH_SpectatorPawn::EditModeON()
{
	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, "ing");

	
	// if (SpectatorPawnMovementComponent)
	// {
	// 	SpectatorPawnMovementComponent->
	// }
	//
	// APlayerController* PlayerController = Cast<APlayerController>(GetController());
	// if (PlayerController)
	// {
	// 	PlayerController->SetIgnoreLookInput(false);
	// }


	// OriginController = Cast<APlayerController>(GetController());
	// 마우스 커서를 보이게 설정
	OriginController->bShowMouseCursor = true;
	SpectatorPawnMovementComponent->SetComponentTickEnabled(true);
	// 커서를 마우스로 제어할 수 있도록 설정
	OriginController->bEnableClickEvents = true;
	OriginController->bEnableMouseOverEvents = true;
}


// =====================================================




void AJSH_SpectatorPawn::EditModeOFF()
{
	if (SpectatorPawnMovementComponent)
	{
		SpectatorPawnMovementComponent->SetComponentTickEnabled(false);
	}
	
	if (OriginController)
	{
		OriginController->SetIgnoreLookInput(false);
		OriginController->bShowMouseCursor = false;
		OriginController->bEnableClickEvents = true;
		OriginController->bEnableMouseOverEvents = true;
		// PlayerController->SetInputMode(FInputModeGameAndUI());
	}

}
	













// void AJSH_SpectatorPawn::NetMulti_BackPlayer_Implementation()
// {
// 	// 1. Get Player Controller (인덱스가 0인 플레이어)
// 	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
//     
// 	if (PlayerController)
// 	{
// 		// 2. Get All Actors of Class (BP_JSH_Player 클래스를 검색)
// 		TArray<AActor*> FoundPlayers;
// 		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AJSH_Player::StaticClass(), FoundPlayers);
// 		
// 		if (FoundPlayers.Num() > 0)
// 		{
// 			// 첫 번째 액터를 사용 ([0]으로 설정)
// 			AJSH_Player* TargetPlayer = Cast<AJSH_Player>(FoundPlayers[0]);
//
// 			if (TargetPlayer)
// 			{
// 				// 3. 플레이어 컨트롤러가 Main Player Possess 하도록
// 				PlayerController->Possess(TargetPlayer);
//
// 				// 4. 자기 자신(SpectatorPawn) Destory
// 				this->Destroy();
//
// 				// 5. Player Mesh visible 꺼줬던거 다시 켜주기
// 				// UMeshComponent* FoundMeshComponent = Cast<UMeshComponent>(TargetPlayer->GetComponentByClass(UMeshComponent::StaticClass()));
// 				// if (FoundMeshComponent)
// 				// {
// 				// 	TargetPlayer->SetVisibility(true);
// 				// }
// 				// 이렇게 하니깐 , Client에서는 visible 켜진게 보여지지 않음 , 그냥 player쪽에서 visible 키는 함수 만들어 두고 , 그걸 가져와야할듯?
// 				
// 				// TargetPlayer->Visible_On_OFF();
// 			}
// 		}
// 	}
// }



