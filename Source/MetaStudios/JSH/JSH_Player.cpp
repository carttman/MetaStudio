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
#include "EntitySystem/MovieSceneEntitySystemRunner.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/SpectatorPawn.h"
#include "GameFramework/SpectatorPawnMovement.h"
#include "Net/UnrealNetwork.h"
#include "UObject/ConstructorHelpers.h"
#include "UObject/UObjectGlobals.h"


//DEFINE_LOG_CATEGORY(LogTemplateCharacter);

#pragma region JSH_PLayer()
AJSH_Player::AJSH_Player()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
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
	
	RecordCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("RecordCamera"));
	RecordCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	RecordCamera->SetRelativeLocation(FVector(440.0f, 0.0f, 0.0f));
	// RecordCamera->SetRelativeRotation(FRotator(13.876860f, -0.433584f, -59.389724f));
	RecordCamera->bUsePawnControlRotation = false;
	RecordCamera->FieldOfView = 90.0f;

	FallGuys = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Body"));
	FallGuys->SetupAttachment(RootComponent);
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/JSH/Asset/FallGuys02/yoshi_-_fall_guys_fan_art.yoshi_-_fall_guys_fan_art'"));
	if (TMesh.Succeeded())
	{
		FallGuys->SetSkeletalMesh(TMesh.Object);
		FallGuys->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, -90, 0));
		FallGuys->SetRelativeScale3D(FVector(0.005, 0.005, 0.005));
	}

	FallGuys_Camera = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Record Camera"));
	FallGuys_Camera->SetupAttachment(FallGuys);
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TMesh2(TEXT("/Script/Engine.SkeletalMesh'/Game/JSH/Asset/Camera03/source/video-cam.video-cam'"));
	if (TMesh2.Succeeded())
	{
		FallGuys_Camera->SetSkeletalMesh(TMesh2.Object);
		FallGuys_Camera->SetRelativeLocationAndRotation(FVector(-12477.217394, 3931.275206, 24551.795870), FRotator(1.727941, 0.148925, 9.851076));
		FallGuys_Camera->SetRelativeScale3D(FVector(100, 100, 100));
	}

	GetCharacterMovement()->MaxFlySpeed = MaxFlySpeed_C;
	GetCharacterMovement()->BrakingDecelerationFlying = BrakingDecelerationFlying_C;
}
#pragma endregion


#pragma region Begin, Multi, Input

void AJSH_Player::BeginPlay()
{
	Super::BeginPlay();

	
	// Record 함수를 끌고 오기 위한 GameInstance 
	ObsGamInstance = Cast<UJSH_OBSWebSocket>(GetGameInstance());


	// 카메라 처음에는 안 보이게
	FallGuys_Camera->SetVisibility(false);


	JPlayerController = Cast<AJSH_PlayerController>(GetWorld()->GetFirstPlayerController());
	
}


// 멀티 BOOL
void AJSH_Player::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AJSH_Player, PlayerVisible_b_On);
	DOREPLIFETIME(AJSH_Player, FlyMode_b_On_Off);
	DOREPLIFETIME(AJSH_Player, Camera_b_Third_First);
	DOREPLIFETIME(AJSH_Player, Record_b_On_Off);
	DOREPLIFETIME(AJSH_Player, EditorMode_B);
	
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

		
		EnhancedInputComponent->BindAction(IA_EditorMode, ETriggerEvent::Started, this, &AJSH_Player::EditorMode);
		
		EnhancedInputComponent->BindAction(IA_Up_Down, ETriggerEvent::Triggered, this, &AJSH_Player::Fly_Up_Down);
		EnhancedInputComponent->BindAction(IA_Up_Down2, ETriggerEvent::Triggered, this, &AJSH_Player::Fly_Down_Ray);
		EnhancedInputComponent->BindAction(IA_MouseWheel, ETriggerEvent::Started, this, &AJSH_Player::FlySpeed);
		
		EnhancedInputComponent->BindAction(IA_Camera_Spawn_Destroy, ETriggerEvent::Started, this, &AJSH_Player::CameraSpawn);
		EnhancedInputComponent->BindAction(IA_Camera_Third_First, ETriggerEvent::Started, this, &AJSH_Player::Camera_Third_First_Change);

		EnhancedInputComponent->BindAction(IA_EditMode, ETriggerEvent::Triggered, this, &AJSH_Player::DisableEdit);
		EnhancedInputComponent->BindAction(IA_EditMode, ETriggerEvent::Ongoing, this, &AJSH_Player::DisableEdit);
		EnhancedInputComponent->BindAction(IA_EditMode, ETriggerEvent::Completed, this, &AJSH_Player::EnableEdit);


		EnhancedInputComponent->BindAction(IA_ZOOM_In, ETriggerEvent::Started, this, &AJSH_Player::Camera_Zoom_In);
		EnhancedInputComponent->BindAction(IA_ZOOM_Out, ETriggerEvent::Started, this, &AJSH_Player::Camera_Zoom_Out);

		EnhancedInputComponent->BindAction(IA_Camera_Right, ETriggerEvent::Triggered, this, &AJSH_Player::CameraRight);
		EnhancedInputComponent->BindAction(IA_Camera_Left, ETriggerEvent::Triggered, this, &AJSH_Player::CameraLeft);
		EnhancedInputComponent->BindAction(IA_Camera_Default, ETriggerEvent::Started, this, &AJSH_Player::CameraDefault);
		
	}
	else
	{
		//UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}
#pragma endregion


#pragma region Move / Look

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

#pragma endregion


#pragma region Editor (Player <-> SpectatorPawn)



// (F) 에디터 모드로 변환하는 함수
void AJSH_Player::SpectatorMode()
{
	// 메인 플랫폼 일떄 기능 LOCK
	if(Bool_MainLock) return;
	
	if (HasAuthority())
	{
		NetMulti_SpectatorMode();
	}
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

		// 원랜 켜야하는데 beginplay에서 해주고 있어서 일단 주석 처리
		//AJSH_PlayerController* PlayerController = Cast<AJSH_PlayerController>(GetWorld()->GetFirstPlayerController());
		

		if (JPlayerController && SpectatorActor)
		{
			APawn* SpectatorPawn = Cast<APawn>(SpectatorActor);
			if (SpectatorPawn)
			{
				JPlayerController->SaveOriginCharacter();
				UE_LOG(LogTemp, Warning, TEXT("11111"));
				// UnPossessed();
				// PlayerController->SetPawn(SpectatorPawn);
				JPlayerController->Possess(SpectatorPawn);
				UGameplayStatics::FinishSpawningActor(SpectatorActor, CameraTransform);
				//FinishSpawning(CameraTransform,true);
				// SpectatorPawn->BeginPlay();
				Visible_On_OFF();
			}
		}
	}

}



// SpecatatorPawn쪽에서 Player Visible 건들려고 하니깐 client쪽에서 안 보여서
// 함수로 따로 뺴뒀음

void AJSH_Player::Visible_On_OFF()
{
	if (HasAuthority())
	{
		NetMulti_Visible_On_OFF();
	}
}

void AJSH_Player::NetMulti_Visible_On_OFF_Implementation()
{
	UE_LOG(LogTemp, Error, TEXT("111111 v on/off"));
	
	//NetMulti_Visible_On_OFF();

	if (PlayerVisible_b_On)
	{
		GetMesh()->SetVisibility(false);
		UE_LOG(LogTemp, Error, TEXT("false"));
	}
	else
	{
		GetMesh()->SetVisibility(true);
		UE_LOG(LogTemp, Error, TEXT("true"));
	}
	
	PlayerVisible_b_On = !PlayerVisible_b_On;

}

#pragma endregion


#pragma region  Record

// 녹화 시작, 종료 함수
void AJSH_Player::StartRecording()
{
	// 메인 플랫폼 일떄 기능 LOCK
	if(Bool_MainLock) return;
	
	if(EditorMode_B) return;
	
	if (HasAuthority())
	{
		// 카메라 확대, 축소, 회전 , 초기화
		CameraReset();
		
		// 인스턴스에 넣어둔 녹화 기능 시작
		//ObsGamInstance->StartRecord();
		JPlayerController->StartRecord();
		NetMulti_StartRecording();
	}

	// NetMulti_StartRecording();
}

void AJSH_Player::NetMulti_StartRecording_Implementation()
{
	if (!Record_b_On_Off)
	{
		if (!FlyMode_b_On_Off) FlyMode();
		
		// 3인칭 -> 1인칭 변환
		FollowCamera->SetActive(false);
		RecordCamera->SetActive(true);
		bUseControllerRotationYaw = true;
		Camera_b_Third_First = true;

		// 카메라 소환
		FallGuys_Camera->SetVisibility(true);
		CameraSpawn_b_On_Off = true;
		UE_LOG(LogTemp, Warning, TEXT("visible true"));

		Record_b_On_Off = true;
	}
	else
	{
		// 1인칭 -> 3인칭 변환
		RecordCamera->SetActive(false);
		FollowCamera->SetActive(true);
		// 비행 상태가 아닐때에만 Yaw를 꺼줌
		if (!GetCharacterMovement()->IsFlying())
		{
			bUseControllerRotationYaw = false;
		}
		Camera_b_Third_First = false;
		
		// 카메라 제거
		FallGuys_Camera->SetVisibility(false);
		CameraSpawn_b_On_Off = false;

		Record_b_On_Off = false;


		
		// Record Mode를 끌때에 아래로 레이 한번 쏴서 , 바닥에 가까우면 Fly 모드 종료
		FVector Start = GetActorLocation();
		FVector End = Start - FVector(0.f, 0.f, 1000.f);

		FHitResult HitResult;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);  // 자기 자신은 충돌 제외

		bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);

		if (bHit)
		{
			float DistanceToGround = HitResult.Distance;

			
			if (DistanceToGround < 100.0f)
			{
				// 거리가 바닥이랑 가까울때 나는 모드면 , FLYMODE()를 가져와서 꺼주고
				if (FlyMode_b_On_Off)
				{
					FlyMode();
				}
				else
				{
					GetCharacterMovement()->SetMovementMode(MOVE_Walking);
				}
			}
			
			DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 1, 0, 5);
		}
		else
		{
			
			DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 1, 0, 5);
		}
	}
}


// 1인칭 <-> 3인칭 ,
void AJSH_Player::Camera_Third_First_Change()
{
	NetMulti_Camera_Third_First_Change();
}

void AJSH_Player::NetMulti_Camera_Third_First_Change_Implementation()
{
	if (!CameraSpawn_b_On_Off)
	{
		// 3인칭 -> 1인칭 변환
		FollowCamera->SetActive(false);
		RecordCamera->SetActive(true);
		bUseControllerRotationYaw = true;
		CameraSpawn_b_On_Off = true;
	}
	else
	{
		// 1인칭 -> 3인칭 변환
		RecordCamera->SetActive(false);
		FollowCamera->SetActive(true);
		// 비행 상태가 아닐때에만 Yaw를 꺼줌
		if (!GetCharacterMovement()->IsFlying())
		{
			bUseControllerRotationYaw = false;
		}
		CameraSpawn_b_On_Off = false;
	}
	Camera_b_Third_First = !Camera_b_Third_First;
}


// 카메라 스폰
void AJSH_Player::CameraSpawn()
{
	NetMulti_CameraSpawn();
}

void AJSH_Player::NetMulti_CameraSpawn_Implementation()
{
	if (!CameraSpawn_b_On_Off)
	{
		FallGuys_Camera->SetVisibility(true);
	}
	else
	{
		FallGuys_Camera->SetVisibility(false);
	}
	
	CameraSpawn_b_On_Off = !CameraSpawn_b_On_Off;
}


#pragma endregion


#pragma region FlyMode


void AJSH_Player::FlySpeed(const FInputActionValue& Value)
{
	// 마우스 우클릭을 누르고 있고(Bool_ZoomMode = true) and Editor Mode가 아니라면 속도를 움직이는게 아니라 카메라 줌인 줌 아웃을 컨트롤
	if (Bool_ZoomMode && !EditorMode_B) return;

	// FlyMode가 아니라면 속도 조절 안 해 !!!
	if (!GetCharacterMovement()->IsFlying()) return;



	float tempvalue = Value.Get<float>();;

	MaxFlySpeed_C = MaxFlySpeed_C + tempvalue * 50;
	
	UE_LOG(LogTemp, Warning, TEXT(" 마우스 휠: %f"), tempvalue);
	UE_LOG(LogTemp, Warning, TEXT(" MaxSpeed: %f"), MaxFlySpeed_C);

	GetCharacterMovement()->MaxFlySpeed = MaxFlySpeed_C;
}



void AJSH_Player::FlyMode()
{
	// 메인 플랫폼 일떄 기능 LOCK
	if(Bool_MainLock) return;
	
	NetMulti_FlyMode();


}
void AJSH_Player::NetMulti_FlyMode_Implementation()
{
	if(!FlyMode_b_On_Off)
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Flying);
		bUseControllerRotationPitch = true;
		bUseControllerRotationYaw = true;
		bUseControllerRotationRoll = true;
	}
	else
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		bUseControllerRotationPitch = false;
		bUseControllerRotationYaw = false;
		bUseControllerRotationRoll = false;
	}

	FlyMode_b_On_Off = !FlyMode_b_On_Off;
}


// (E) 위로 올라가는 - 레이 안 쏨
void AJSH_Player::Fly_Up_Down(const FInputActionValue& Value)
{
	NetMulti_Fly_Up_Down(Value);
}

void AJSH_Player::NetMulti_Fly_Up_Down_Implementation(const FInputActionValue& Value)
{
	// if (GetCharacterMovement()->IsFlying())
	// {
	// 	// 입력 값에서 Up/Down 액션 값 추출
	// 	Fly_Zvalue = Value.Get<float>();
	// 	UE_LOG(LogTemp, Warning, TEXT("처음: %f"), Fly_Zvalue)
	// 	AddMovementInput(FVector(0.f, 0.f, 1.f), Fly_Zvalue);
	//
	// 	Fly_Off_Value = Fly_Off_Value + Fly_Zvalue;
	//
	// 	if (Fly_Off_Value <= -10)
	// 	{
	// 		FlyMode();
	// 		Fly_Off_Value = 0;
	// 	}
	// }
	if (!FlyMode_b_On_Off)
	{
		FlyMode();	
	}
	if (GetCharacterMovement()->IsFlying())
	{
		// 입력 값에서 Up/Down 액션 값 추출
		Fly_Zvalue = Value.Get<float>();
		UE_LOG(LogTemp, Warning, TEXT("처음: %f"), Fly_Zvalue)
		AddMovementInput(FVector(0.f, 0.f, 1.f), Fly_Zvalue);
	}
}


// (Q) 아래로 내려가는 -> 내려갈때 레이쏴서 일정 거리 가까워지면 FlyMode 종료 
void AJSH_Player::Fly_Down_Ray(const FInputActionValue& Value)
{
	NetMulti_Fly_Down_Ray(Value);
}



void AJSH_Player::NetMulti_Fly_Down_Ray_Implementation(const FInputActionValue& Value)
{
	
	if (GetCharacterMovement()->IsFlying())
	{
		
		// 입력 값에서 Up/Down 액션 값 추출
		Fly_Zvalue = Value.Get<float>();
		AddMovementInput(FVector(0.f, 0.f, 1.f), Fly_Zvalue);

		// 아래에 있는 물체와의 거리 체크
		FVector Start = GetActorLocation();
		FVector End = Start - FVector(0.f, 0.f, 1000.f);

		FHitResult HitResult;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);  // 자기 자신은 충돌 제외

		bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);

		if (bHit)
		{
			float DistanceToGround = HitResult.Distance;

			// Editor 모드가 아닐때 or Record Mode가 아닐때 Fly Mode를 종료
			if (!EditorMode_B & !Record_b_On_Off)
			{
				// 원하는 거리 임계값 설정
				if (DistanceToGround < 100.0f)
				{
					FlyMode(); 
				}
			}
			
			
			DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 1, 0, 5);
		}
		else
		{
			
			DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 1, 0, 5);
		}
	}

}

#pragma endregion


#pragma region EditorMode


// (key: 1) EditorMode On / Off
void AJSH_Player::EditorMode()
{
	// 메인 플랫폼 일떄 기능 LOCK
	if(Bool_MainLock) return;
	
	// 녹화 중이 아닐때에만 Editor 모드 가능
	if (Record_b_On_Off) return;
	
	
	NetMulti_EditorMode();
}

void AJSH_Player::NetMulti_EditorMode_Implementation()
{
	// Editor Mode On
	if (!EditorMode_B)
	{
		// 카메라 확대, 축소, 회전 , 초기화
		CameraReset();
		
		UE_LOG(LogTemp, Error, TEXT("EditorModeOn"));
		
		// FlyMode를 제어하는 bool 값 (Editor Mode 일때 항상 날아다니 도록)
		EditorMode_B = true;
		
		// 3인칭 -> 1인칭 변환
		FollowCamera->SetActive(false);
		RecordCamera->SetActive(true);
		bUseControllerRotationYaw = true;
		Camera_b_Third_First = true;

		// E를 누르지 않아도 임시 Fly Mode 
		GetCharacterMovement()->SetMovementMode(MOVE_Flying);
		bUseControllerRotationPitch = true;
		bUseControllerRotationYaw = true;
		bUseControllerRotationRoll = true;

		EnableEdit();
	}
	// Editor Mode Off
	else
	{
		UE_LOG(LogTemp, Error, TEXT("EditorMode Off"));
		DisableEdit();
		
		// FlyMode를 제어하는 bool 값 (Editor Mode 일때 항상 날아다니 도록)
		EditorMode_B = false;
		
		// 1인칭 -> 3인칭 변환
		RecordCamera->SetActive(false);
		FollowCamera->SetActive(true);
		// 비행 상태가 아닐때에만 Yaw를 꺼줌
		if (!GetCharacterMovement()->IsFlying())
		{
			bUseControllerRotationYaw = false;
			bUseControllerRotationPitch = false;
			bUseControllerRotationRoll = false;
		}
		Camera_b_Third_First = false;


		// Fly Mode를 끌때에 아래로 레이 한번 쏴서 , Fly 모드 종료
		FVector Start = GetActorLocation();
		FVector End = Start - FVector(0.f, 0.f, 1000.f);

		FHitResult HitResult;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);  // 자기 자신은 충돌 제외

		bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);

		if (bHit)
		{
			float DistanceToGround = HitResult.Distance;

			
			if (DistanceToGround < 100.0f)
			{
				// 거리가 바닥이랑 가까울때 나는 모드면 , FLYMODE()를 가져와서 꺼주고
				if (FlyMode_b_On_Off)
				{
					FlyMode();
				}
				// Fly Mode가 아닌데 바닥이랑 가까우면 요렇게 꺼줌
				else
				{
					GetCharacterMovement()->SetMovementMode(MOVE_Walking);
				}
			}
			
			DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 1, 0, 5);
		}
		else
		{
			
			DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 1, 0, 5);
		}
	}
}


void AJSH_Player::EnableEdit()
{
	// 마우스 우클릭을 안 하고 있으면 , ZoomMode를 Flase로
	Bool_ZoomMode = false;
	UE_LOG(LogTemp, Error, TEXT("false"));
	
	// Editor 모드에서 마우스를 누른 상태로 , Editor 모드를 껏을때 , 꺼지고 나서 마우스를 떼면 EnableEdit가 On되는 문제가 있어
	// 이렇게 막아줌
	if (!EditorMode_B) return;


	
	GetMovementComponent()->SetComponentTickEnabled(false);
	
	if (JPlayerController)
	{
		// 마우스 커서 보이게 설정
		JPlayerController->bShowMouseCursor = true;
		JPlayerController->bEnableClickEvents = true;
		JPlayerController->bEnableMouseOverEvents = true;
		
		// 마우스를 UI와 게임에서 사용할 수 있도록 설정
		JPlayerController->SetInputMode(FInputModeGameAndUI());
		GEngine->GameViewport->SetMouseLockMode(EMouseLockMode::LockAlways);
	}
}

void AJSH_Player::DisableEdit()
{
	// 마우스 우클릭 하고 있으면, ZoomMode를 True로
	if (!Bool_ZoomMode)
	{
		Bool_ZoomMode = true;
		UE_LOG(LogTemp, Error, TEXT("true"));
	}
	
	GetMovementComponent()->SetComponentTickEnabled(true);

	if (JPlayerController)
	{
		JPlayerController->SetIgnoreLookInput(false);
		
		JPlayerController->bShowMouseCursor = false;
		JPlayerController->bEnableClickEvents = false;
		JPlayerController->bEnableMouseOverEvents = false;
		JPlayerController->SetInputMode(FInputModeGameOnly());
	}
}


#pragma endregion
	

#pragma region Camera Control

void AJSH_Player::Camera_Zoom_In()
{
	// 마우스 우클릭을 누르고 있고(Bool_ZoomMode = true) and Editor Mode가 아니라면 속도를 움직이는게 아니라 카메라 줌인 줌 아웃을 컨트롤
	if (Bool_ZoomMode && !EditorMode_B)
	{
		// CameraBoom->TargetArmLength = FMath::Clamp<float>(CameraBoom->TargetArmLength+30.0f, 150.0f, 800.0f);
		UE_LOG(LogTemp, Error, TEXT("Out"));
		
		// float NewFOV = FMath::Clamp(RecordCamera->FieldOfView + ZoomSpeed, MinFOV, MaxFOV);
		ZoomFOV = RecordCamera->FieldOfView - ZoomSpeed;
		if (ZoomFOV <= 10.0f)
		{
			ZoomFOV = 10.0f;
			RecordCamera->SetFieldOfView(ZoomFOV);
		}
		else
		{
			RecordCamera->SetFieldOfView(ZoomFOV);
		}
	}
}

void AJSH_Player::Camera_Zoom_Out()
{
	// 마우스 우클릭을 누르고 있고(Bool_ZoomMode = true) and Editor Mode가 아니라면 속도를 움직이는게 아니라 카메라 줌인 줌 아웃을 컨트롤
	
	if (Bool_ZoomMode && !EditorMode_B)
	{
		// CameraBoom->TargetArmLength = FMath::Clamp<float>(CameraBoom->TargetArmLength-30.0f, 150.0f, 800.0f);
		UE_LOG(LogTemp, Error, TEXT("IN"));

		// float NewFOV = FMath::Clamp(RecordCamera->FieldOfView - ZoomSpeed, MinFOV, MaxFOV);
		
		ZoomFOV = RecordCamera->FieldOfView + ZoomSpeed;
		if (ZoomFOV >= 120.0f)
		{
			ZoomFOV = 120.0f;
			RecordCamera->SetFieldOfView(ZoomFOV);
		}
		else
		{
			RecordCamera->SetFieldOfView(ZoomFOV);
		}
	}
}

void AJSH_Player::CameraRight()
{
	// 1인칭이 아니라면 실행 하지 않음
	if (!RecordCamera->IsActive()) return;
	
	CurrentAngl += Amount;
	
	if (CurrentAngl > 45.0f) 
	{
		CurrentAngl = 45.0f;
	}
	
	NewCameraRotation = RecordCamera->GetRelativeRotation();
	NewCameraRotation.Roll = CurrentAngl;
	RecordCamera->SetRelativeRotation(NewCameraRotation);
}

void AJSH_Player::CameraLeft()
{
	// 1인칭이 아니라면 실행 하지 않음
	if (!RecordCamera->IsActive()) return;
	
	CurrentAngl -= Amount;
	
	if (CurrentAngl < -45.0f) 
	{
		CurrentAngl = -45.0f;
	}
	
	NewCameraRotation = RecordCamera->GetRelativeRotation();
	NewCameraRotation.Roll = CurrentAngl;
	RecordCamera->SetRelativeRotation(NewCameraRotation);
}

void AJSH_Player::CameraDefault()
{
	// 1인칭이 아니라면 실행 하지 않음
	if (!RecordCamera->IsActive()) return;
	
	RecordCamera->SetRelativeRotation(DefaultCameraleaning);
	CurrentAngl = 0;
}

void AJSH_Player::CameraReset()
{
	// 카메라 원상 복구 해주는 코드
	RecordCamera->SetFieldOfView(90.0f);
	RecordCamera->SetRelativeRotation(DefaultCameraleaning);
	CurrentAngl = 0;
}

#pragma endregion





#pragma region Text

// 녹화 모드 시작 / 종료 (R)

// 녹화 모드 on / ing 일떄
// 1) 1인칭 변경
// 2) 마우스 회전에 따라 시점 , 몸의 방향 움직임
// [마우스를 바닥으로 내리면(화면 시점이 바닥으로 가고 캐릭터의 몸도 0---< 이런식으로 바닥 ]
// 3) 캐릭터 주위에 카메라 모형 생성
// 4) 녹화 모드 시작 하자 마자 FlyMode ON (Recrod 모드일때 항상 FlyMode)
// 5) 날면서 아래로 하강 하여 바닥에 닿아도 FlyMode OFF 되지 않음 (아래에 있는 fly 모드 매커니즘 확인 필요)

// 녹화 모드 종료 시
// 1) 3인칭 -> 1인칭
// 2) 마우스 회전에 따라 시점 , 몸의 방향 움직임 -> 일반적인 third person 카메라 회전 및 움직임
// 3) 캐릭터 모형 제거
// 4) 녹화 종료 버튼 눌렀을떄에 하늘에 날고 있다면 그대로 FlyMode 유지, 바닥과 아주 가까이 있다면 ? walk 모드로 변경(fly mode off)

// + Record 모드일떄 Editor 모드 on 불가능


// 위로 상승 (E) , 아래로 하강 (Q)

// 1) 기본적으로 wasd로 걸어다니가가 e를 누르면 하늘로 몸 상승(꾸욱 누르고 있음 계속 올라가고 , 누르는 키를 떄면 올라가는거 멈춤)
// 2) 몸이 공중에 있을때에 q를 누르면 아래로 하강 (꾸욱 누르고 있음 계속 내려가고 , 누르는 키를 떄면 올라가는거 멈춤)
// 3) 하강 시 플레이어 아래에 있는 물체와 거리가 아주 가까워 지면 flymode 종료 -> walk mode로 돌아감
// 4) 마우스 회전에 따라 시점 , 몸의 방향 움직임
// [마우스를 바닥으로 내리면(화면 시점이 바닥으로 가고 캐릭터의 몸도 0---< 이런식으로 바닥 ]




// Editor Mode ON / OFF (Z or 1 , 키 는 고민 중 에디터 모드 내에서 단축키 설정에 따라 바꾸려고 고민 중 )

//Editor on / ing 일떄
// 1) Editor 모드 시작시 1인칭 변경
// 2) 마우스 회전에 따라 시점 , 몸의 방향 움직임
// [마우스를 바닥으로 내리면(화면 시점이 바닥으로 가고 캐릭터의 몸도 0---< 이런식으로 바닥 ]
// 3) 캐릭터 주위에 공구 모형 생성 (예정) , Editor 모드에서 에셋 배치하려고 클릭하고 있을떄에 애니메이션 모션 등등 (예정)
// 4) Editor 시작 하자 마자 FlyMode ON (Editor 모드일때 항상 FlyMode)
// 5) 날면서 아래로 하강 하여 바닥에 닿아도 FlyMode OFF 되지 않음 (아래에 있는 fly 모드 매커니즘 확인 필요)
// 6) 언리얼 에디터와 동일하게 editor 모드가 켜졌을때에는 마우스 우클릭을 하고 있어야만 플레이어를 움직일 수 있음 (마우스 누르고 있을떄 마우스 포인터 없어짐)
// 7) 마우스 우클릭을 하고 있지 않다면 마우스 포인터가 켜짐 , ui에 있는 물체들을 클릭하고 끌고 올 수 있게 됨

// Editor 모드 종료 시
// 1) 3인칭 -> 1인칭
// 2) 마우스 회전에 따라 시점 , 몸의 방향 움직임 -> 일반적인 third person 카메라 회전 및 움직임
// 3) 공구 모형 제거 (예정)
// 4) Editor 종료 버튼 눌렀을떄에 하늘에 날고 있다면 그대로 FlyMode 유지, 바닥과 아주 가까이 있다면 ? walk 모드로 변경(fly mode off)

// + Editor 모드일떄 record 모드 on 불가능
#pragma endregion