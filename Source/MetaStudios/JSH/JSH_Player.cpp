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
#include "GameFramework/Pawn.h"
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

	// ObsGamInstance = Cast<UJSH_OBSWebSocket>(GetGameInstance());
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


// void AJSH_Player::ConnectToOBS()
// {
	// if (!FModuleManager::Get().IsModuleLoaded("WebSockets"))
	// {
	// 	FModuleManager::Get().LoadModuleChecked("WebSockets");
	// }
	//
	// WebSocket = FWebSocketsModule::Get().CreateWebSocket("ws://192.168.0.4:4455");
	//
	// WebSocket->Connect();
// }
//
// void AJSH_Player::DisConnectToOBS()
// {
	// if (HasAuthority())
	// {
	// 	if (WebSocket->IsConnected())
	// 	{
	// 		WebSocket->Close();
	// 	}
	// }
// }

void AJSH_Player::SpectatorMode()
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

	// 3. 블루프린트 클래스 로드 -> Possess 바꿔주기 + Visible 끄기
	UClass* SpectatorClass = StaticLoadClass(AActor::StaticClass(), nullptr, TEXT("/Game/JSH/BP/BP_Spectator.BP_Spectator_C"));
    
	if (SpectatorClass)
	{
		AActor* SpectatorActor = GetWorld()->SpawnActor<AActor>(SpectatorClass, CameraTransform, SpawnParams);
		
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
        
		if (PlayerController && SpectatorActor)
		{
			
			APawn* SpectatorPawn = Cast<APawn>(SpectatorActor);
			if (SpectatorPawn)
			{
				PlayerController->Possess(SpectatorPawn);
				
				GetMesh()->SetVisibility(false); 
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Spectator class could not be loaded!"));
	}
}



// 녹화 시작 함수
void AJSH_Player::StartRecording()
{
	// 테스트 할때 문제 없게, 일단 서버만 허용
	if (HasAuthority())
	{
		if (!Recording)
		{
			// 녹화 시작 코드 
			// FString Command = TEXT("ffmpeg -f dshow -i audio=\"Line 1(Virtual Audio Cable)\" -f gdigrab -offset_x 0 -offset_y 0 -video_size 1920x1080 -framerate 30 -draw_mouse 1 -probesize 1000M -i desktop -c:v h264_nvenc -qp 0 C:\\Users\\jsh\\Documents\\GitHub\\MetaStudio\\ffmpeg-7.1-full_build\\UE_Recording_Screen_with_Audio.mkv");
			//
			//
			// FString ExecutablePath = TEXT("C:/Users/jsh/Documents/GitHub/MetaStudio/ffmpeg-7.1-full_build/bin/ffmpeg.exe");  // Path ffmpeg.exe
			// FString Params = TEXT("-f dshow -i audio=\"Line 1(Virtual Audio Cable)\" -f gdigrab -offset_x 0 -offset_y 0 -video_size 1920x1080 -framerate 30 -draw_mouse 1 -probesize 1000M -i desktop -c:v h264_nvenc -qp 0 C:\\Users\\jsh\\Documents\\GitHub\\MetaStudio\\ffmpeg-7.1-full_build\\UE_Recording_Screen_with_Audio.mkv");
			

			
			FDateTime Now = FDateTime::Now();
			FString DateTimeString = Now.ToString(TEXT("%Y-%m-%d_%H-%M-%S")); 

			
			FString FilePath = FString::Printf(TEXT("C:\\Users\\jsh\\Documents\\GitHub\\MetaStudio\\ffmpeg-7.1-full_build\\UE_Recording_Screen_with_Audio_%s.mkv"), *DateTimeString);

			FString ExecutablePath = TEXT("C:/Users/jsh/Documents/GitHub/MetaStudio/ffmpeg-7.1-full_build/bin/ffmpeg.exe");  // Path to ffmpeg.exe
			FString Params = FString::Printf(TEXT("-f dshow -i audio=\"Line 1(Virtual Audio Cable)\" -f gdigrab -offset_x 0 -offset_y 0 -video_size 1920x1080 -framerate 30 -draw_mouse 1 -probesize 1000M -i desktop -c:v h264_nvenc -qp 0 %s"), *FilePath);

			
			FProcHandle ProcessHandle = FPlatformProcess::CreateProc(
				*ExecutablePath, // Executable path
				*Params,         // Command parameters
				true,            // bLaunchDetached (true to run in the background)
				false,           // bLaunchHidden (false if you want to see the console window)
				false,           // bLaunchReallyHidden (same as above)
				nullptr,         // OutProcessID
				0,               // Priority
				nullptr,         // Optional working directory
				nullptr          // Pipe write to nullptr (not needed)
			);

			PH = ProcessHandle;
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, "Start Recording");
			
			if (ProcessHandle.IsValid())
			{
				UE_LOG(LogTemp, Log, TEXT("Recording started successfully in the background"));
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to start recording process."));
			}
		}
		else
		{
			if (PH.IsValid())
			{
				
				FPlatformProcess::TerminateProc(PH);
				FPlatformProcess::CloseProc(PH);  
	        
				UE_LOG(LogTemp, Log, TEXT("Recording stopped and FFmpeg process terminated"));
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to stop recording. No valid process handle found."));
			}
			GEngine->AddOnScreenDebugMessage(-2, 10.0f, FColor::Yellow, "End Recording");
		}
		Recording = !Recording;
	}


	

	// if (HasAuthority())
	// {
	// 	Jump();
	// }
}

	

	

// 녹화 종료 함수
void AJSH_Player::StopRecording()
{
	// 테스트 할때 문제 없게, 일단 서버만 허용
	if (HasAuthority())
	{
		if (PH.IsValid())
		{
			FPlatformProcess::TerminateProc(PH);
			FPlatformProcess::CloseProc(PH);  
        
			UE_LOG(LogTemp, Log, TEXT("Recording stopped and FFmpeg process terminated"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to stop recording. No valid process handle found."));
		}
	}

	
	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, "End Recording");
}