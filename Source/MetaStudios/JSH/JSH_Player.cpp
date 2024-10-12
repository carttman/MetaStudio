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

	ObsGamInstance = Cast<UJSH_OBSWebSocket>(GetGameInstance());

	
	// if (HasAuthority())
	// {
	// 	if (!FModuleManager::Get().IsModuleLoaded("WebSockets"))
	// 	{
	// 		FModuleManager::Get().LoadModuleChecked("WebSockets");
	// 	}
	//
	// 	WebSocket = FWebSocketsModule::Get().CreateWebSocket("ws://192.168.0.4:4455");
	// 	
	// 	WebSocket->Connect();
	// }
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
		EnhancedInputComponent->BindAction(StopRecord, ETriggerEvent::Started, this, &AJSH_Player::StopRecording);
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


void AJSH_Player::ConnectToOBS()
{
	// if (!FModuleManager::Get().IsModuleLoaded("WebSockets"))
	// {
	// 	FModuleManager::Get().LoadModuleChecked("WebSockets");
	// }
	//
	// WebSocket = FWebSocketsModule::Get().CreateWebSocket("ws://192.168.0.4:4455");
	//
	// WebSocket->Connect();
}

void AJSH_Player::DisConnectToOBS()
{
	// if (HasAuthority())
	// {
	// 	if (WebSocket->IsConnected())
	// 	{
	// 		WebSocket->Close();
	// 	}
	// }
}


// 녹화 시작 함수
void AJSH_Player::StartRecording()
{
	// if (IsLocallyControlled() && !HasAuthority())
	// {
	// 	GEngine->AddOnScreenDebugMessage(0, 2, FColor::Green, TEXT("recording"));
	// 	Jump();
	// }

	if (HasAuthority())
	{
		ObsGamInstance->StartRecord();
		Jump();
	}
	
	// if (OBSWebSocket.IsValid() && OBSWebSocket->IsConnected())
	// {
	// 	// 녹화 시작 명령 전송
	// 	FString StartRecordingMessage = TEXT("{\"op\": 6, \"d\": {\"requestType\": \"StartRecording\", \"requestId\": \"startRecording\"}}");
	// 	OBSWebSocket->Send(StartRecordingMessage);
	// 	UE_LOG(LogTemp, Log, TEXT("Sent StartRecording Command to OBS"));
	// }
}

// 녹화 종료 함수
void AJSH_Player::StopRecording()
{
	// if (OBSWebSocket.IsValid() && OBSWebSocket->IsConnected())
	// {
	// 	// 녹화 종료 명령 전송
	// 	FString StopRecordingMessage = TEXT("{\"op\": 6, \"d\": {\"requestType\": \"StopRecording\", \"requestId\": \"stopRecording\"}}");
	// 	OBSWebSocket->Send(StopRecordingMessage);
	// 	UE_LOG(LogTemp, Log, TEXT("Sent StopRecording Command to OBS"));
	// }
}