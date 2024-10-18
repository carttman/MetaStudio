// Fill out your copyright notice in the Description page of Project Settings.


#include "JSH_Player.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "IWebSocket.h"
#include "JSH_OBSWebSocket.h"
#include "WebSocketsModule.h"
#include <cstdlib>

#include "DelayAction.h"
#include "JSH_PlayerController.h"
#include "K2Node_SpawnActorFromClass.h"
#include "Misc/Paths.h"
#include "HAL/PlatformProcess.h"
#include "Misc/CommandLine.h"
#include "HAL/PlatformFilemanager.h"
#include "Kismet/GameplayStatics.h"
#include "UniversalObjectLocators/UniversalObjectLocatorUtils.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SceneComponent.h"
#include "Engine/Engine.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/SpectatorPawn.h"
#include "Net/UnrealNetwork.h"
#include "UObject/ConstructorHelpers.h"
#include "UObject/UObjectGlobals.h"


//DEFINE_LOG_CATEGORY(LogTemplateCharacter);


AJSH_Player::AJSH_Player()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); 

	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; 
	CameraBoom->bUsePawnControlRotation = true; 

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); 
	FollowCamera->bUsePawnControlRotation = false; 
}

void AJSH_Player::BeginPlay()
{
	Super::BeginPlay();

	
	// Record 함수를 끌고 오기 위한 GameInstance 
	ObsGamInstance = Cast<UJSH_OBSWebSocket>(GetGameInstance());
}


// 멀티 BOOL
void AJSH_Player::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AJSH_Player, PlayerVisibleOn);
}


void AJSH_Player::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		

		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);


		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AJSH_Player::Move);


		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AJSH_Player::Look);

		EnhancedInputComponent->BindAction(StartRecord, ETriggerEvent::Started, this, &AJSH_Player::StartRecording);
		EnhancedInputComponent->BindAction(SpectatorModeOnOff, ETriggerEvent::Started, this, &AJSH_Player::SpectatorMode);
	}
	else
	{
		//UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AJSH_Player::Move(const FInputActionValue& Value)
{

	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{

		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AJSH_Player::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}


#pragma region Player <-> SpectatorPawn

// (F) 에디터 모드로 변환하는 함수
void AJSH_Player::SpectatorMode()
{
	NetMulti_SpectatorMode();
	
}

void AJSH_Player::NetMulti_SpectatorMode_Implementation()
{
	// Player 모드 변환 인풋 (F) -> Spectator Actor 스폰 후 Possess 바꿈
	// Spectator Actor에서 다시 Possess 바꿔줘야 (F) -> Player로 다시 돌아올 수 있음
	// @ 움직이다가 모드 바꿔주면, 클라이언트 쪽에서 모드 바꾸기 직전에 들어가던 인풋이 계속 들어가는 것처럼 보임, 멀티 쏴 줘야 할듯
	
	// 1. Player 카메라 위치 가져오기
	FTransform CameraTransform;
	if (FollowCamera)
	{
		CameraTransform = FollowCamera->GetComponentTransform();
	}

	// 2. BP_Spectator를 FollowCamera 위치에 스폰하기
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	
	AGameModeBase* tt = Cast<AGameModeBase>(GetWorld()->GetAuthGameMode());
	
	
	// 3. 블루프린트 클래스 로드 -> Possess 바꿔주기 + Visible 끄기
	UClass* SpectatorClass = StaticLoadClass(AActor::StaticClass(), nullptr, TEXT("/Game/JSH/BP/BP_Spectator.BP_Spectator_C"));
    
	if (SpectatorClass)
	{
		//AActor* SpectatorActor = GetWorld()->SpawnActor<AActor>(SpectatorClass, CameraTransform, SpawnParams);
		//ASpectatorPawn* SpectatorActor = GetWorld()->SpawnActor<ASpectatorPawn>(tt->SpectatorClass, CameraTransform, SpawnParams);
		AActor* SpectatorActor = GetWorld()->SpawnActorDeferred<AActor>(SpectatorClass, CameraTransform);
		AJSH_PlayerController* PlayerController = Cast<AJSH_PlayerController>(GetWorld()->GetFirstPlayerController());
		
        
		if (PlayerController && SpectatorActor)
		{
			APawn* SpectatorPawn = Cast<APawn>(SpectatorActor);
			if (SpectatorPawn)
			{
				if(HasAuthority())
				{
					PlayerController->SaveOriginCharacter();
					UE_LOG(LogTemp, Warning, TEXT("11111"));
					// UnPossessed();
					// PlayerController->SetPawn(SpectatorPawn);
					PlayerController->Possess(SpectatorPawn);
					UGameplayStatics::FinishSpawningActor(SpectatorActor, CameraTransform);
					//FinishSpawning(CameraTransform,true);
					// SpectatorPawn->BeginPlay();
				}
				Visible_On_OFF();
			}
		}
	}
}



// SpecatatorPawn쪽에서 Player Visible 건들려고 하니깐 client쪽에서 안 보여서
// 함수로 따로 뺴뒀음
void AJSH_Player::Visible_On_OFF()
{
	NetMulti_Visible_On_OFF_Implementation();
}

void AJSH_Player::NetMulti_Visible_On_OFF_Implementation()
{
	//NetMulti_Visible_On_OFF();
	if (PlayerVisibleOn)
	{
		GetMesh()->SetVisibility(false, true);
	}
	else
	{
		GetMesh()->SetVisibility(true, true);
	}
	PlayerVisibleOn = !PlayerVisibleOn;
}

#pragma endregion




#pragma region  Record

// 녹화 시작, 종료 함수
void AJSH_Player::StartRecording()
{
	if (HasAuthority())
	{
		ObsGamInstance->StartRecord();
	}
}

#pragma endregion



