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
#include "JSH_OBSWebSocket.h"
#include <cstdlib>

#include "JSH_Editor_SpawnActor.h"
#include "JSH_PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Misc/Paths.h"
#include "HAL/PlatformProcess.h"
#include "Misc/CommandLine.h"
#include "HAL/PlatformFilemanager.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "Components/SceneComponent.h"
#include "Engine/Engine.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/SpectatorPawnMovement.h"
#include "Gizmo/JSH_Scale_GizmoX.h"
#include "Gizmo/JSH_Scale_GizmoY.h"
#include "Gizmo/JSH_Scale_GizmoZ.h"
#include "Gizmo/JSH_Translate_GizmoX.h"
#include "Gizmo/JSH_Translate_GizmoY.h"
#include "Gizmo/JSH_Translate_GizmoZ.h"
#include "MetaStudios/CHJ/MainGameInstance.h"
#include "MetaStudios/CHJ/Component/FirebaseComponent.h"
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
	RecordCamera->SetRelativeLocation(FVector(460.0f, 0.0f, 0.0f));
	// RecordCamera->SetRelativeRotation(FRotator(13.876860f, -0.433584f, -59.389724f));
	RecordCamera->bUsePawnControlRotation = false;
	RecordCamera->FieldOfView = 90.0f;

	// FallGuys = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Body"));
	// FallGuys->SetupAttachment(RootComponent);
	// ConstructorHelpers::FObjectFinder<USkeletalMesh> TMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/JSH/Asset/FallGuys02/yoshi_-_fall_guys_fan_art.yoshi_-_fall_guys_fan_art'"));
	// if (TMesh.Succeeded())
	// {
	// 	FallGuys->SetSkeletalMesh(TMesh.Object);
	// 	FallGuys->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, -90, 0));
	// 	FallGuys->SetRelativeScale3D(FVector(0.005, 0.005, 0.005));
	// 	FallGuys->SetCastShadow(true);
	// }
	//
	//
	// FallGuys_Camera = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Record Camera"));
	// FallGuys_Camera->SetupAttachment(FallGuys);
	// ConstructorHelpers::FObjectFinder<USkeletalMesh> TMesh2(TEXT("/Script/Engine.SkeletalMesh'/Game/JSH/Asset/Camera03/source/video-cam.video-cam'"));
	// if (TMesh2.Succeeded())
	// {
	// 	FallGuys_Camera->SetSkeletalMesh(TMesh2.Object);
	// 	FallGuys_Camera->SetRelativeLocationAndRotation(FVector(-12477.217394, 3931.275206, 24551.795870), FRotator(1.727941, 0.148925, 9.851076));
	// 	FallGuys_Camera->SetRelativeScale3D(FVector(100, 100, 100));
	// }


	FallGuys = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Body"));
	FallGuys->SetupAttachment(RootComponent);
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/JSH/Asset/Corgi/untitled.untitled'"));
	if (TMesh.Succeeded())
	{
		FallGuys->SetSkeletalMesh(TMesh.Object);
		FallGuys->SetRelativeLocationAndRotation(FVector(20, 0, -12), FRotator(0, -90, 0));
		FallGuys->SetRelativeScale3D(FVector(0.5, 0.5, 0.5));
		FallGuys->SetCastShadow(true);
		
		//FallGuys->SetVisibility(false);
	}
	

	FallGuys_Camera = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Record Camera"));
	FallGuys_Camera->SetupAttachment(FallGuys);
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TMesh2(TEXT("/Script/Engine.SkeletalMesh'/Game/JSH/Asset/Camera03/source/video-cam.video-cam'"));
	if (TMesh2.Succeeded())
	{
		FallGuys_Camera->SetSkeletalMesh(TMesh2.Object);
		FallGuys_Camera->SetRelativeLocationAndRotation(FVector(-12477.217394, 3931.275206, 24551.795870), FRotator(1.727941, 0.148925, 9.851076));
		FallGuys_Camera->SetRelativeScale3D(FVector(100, 100, 100));

		FallGuys->SetVisibility(false);
	}

	
	GetCharacterMovement()->MaxFlySpeed = MaxFlySpeed_C;
	GetCharacterMovement()->BrakingDecelerationFlying = BrakingDecelerationFlying_C;

	FirebaseComponent = CreateDefaultSubobject<UFirebaseComponent>(TEXT("FirebaseComponent"));



	Root_Camera01 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Root_Camera01"));
	Root_Camera01->SetupAttachment(RootComponent);
	Root_Camera01->SetRelativeLocation(FVector(70.f, -74.f, -265.f));
	Root_Camera01->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));
	Root_Camera01->SetRelativeScale3D(FVector(1.5f, 1.5f, 1.5f));
	
	Camera02 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Camera02"));
	Camera02->SetupAttachment(Root_Camera01);

	Camera03 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Camera03"));
	Camera03->SetupAttachment(Root_Camera01);

	Camera04 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Camera04"));
	Camera04->SetupAttachment(Root_Camera01);

	Camera05 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Camera05"));
	Camera05->SetupAttachment(Root_Camera01);

	Camera06 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Camera06"));
	Camera06->SetupAttachment(Root_Camera01);

	Camera07 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Camera07"));
	Camera07->SetupAttachment(Root_Camera01);
}
#pragma endregion


#pragma region Begin, Multi, Input

void AJSH_Player::BeginPlay()
{
	Super::BeginPlay();


	// FString RelativePath = FPaths::ProjectContentDir();
	//
	// FString FullPath = IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(*RelativePath);
	// IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(*RelativePath);
	// UE_LOG(LogTemp, Error, TEXT("RelativePath: %s"), *RelativePath);
	// UE_LOG(LogTemp, Error, TEXT("FullPath: %s"), *FullPath);
	//
	// // Record 함수를 끌고 오기 위한 GameInstance 
	// ObsGamInstance = Cast<UJSH_OBSWebSocket>(GetGameInstance());
	// CHJ_Instance = Cast<UMainGameInstance>(GetGameInstance());

	
	// // 플레이어 컨트롤러
	// JPlayerController = Cast<AJSH_PlayerController>(GetWorld()->GetFirstPlayerController());
	// if (JPlayerController)
	// {
	// 	JPlayerController->bEnableTouchEvents = false;
	// 	
	// 	UE_LOG(LogTemp, Error, TEXT("Begin_Jcontorller1111"));
	// 	// 플레이어 컨트롤러에 Director 저장
	// 	JPlayerController->SaveOriginCharacter();
	// }

	FlyMode();
	
	// 캐릭터 없어져서 그냥 바로 1인칭 시작
	FollowCamera->SetActive(false);
	RecordCamera->SetActive(true);
	bUseControllerRotationYaw = true;
	CameraSpawn_b_On_Off = true;
}

void AJSH_Player::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// if (Clicked)
	// {
	// 	Gizmo_Click();
	// }

	
	// 기즈모가 선택되어 있지 않을때 레이를 통해 기즈모를 감지
	// 기즈모가 선택 모드가 아니고 EnableEdit 모드일때만 Tick 돌게
	if (!Gizmo_Selected && EnableEditSystem)
	{
		Gizmo_Detect();
	}
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
	DOREPLIFETIME(AJSH_Player, ClickedEditorActor);
	DOREPLIFETIME(AJSH_Player, Bool_EditorActorDestroy);
	DOREPLIFETIME(AJSH_Player, DisableEdit_b);
	DOREPLIFETIME(AJSH_Player, DisableEdit2_b);
}

void AJSH_Player::Saved_PlayerController()
{
	JPlayerController = Cast<AJSH_PlayerController>(GetWorld()->GetFirstPlayerController());
	if (JPlayerController)
	{
		JPlayerController->bEnableTouchEvents = false;
		
		UE_LOG(LogTemp, Error, TEXT("Begin_Jcontorller1111"));
		// 플레이어 컨트롤러에 Director 저장
		JPlayerController->SaveOriginCharacter();
	}
}


void AJSH_Player::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
			UE_LOG(LogTemp, Error, TEXT("director add mapping"));
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

		// 카메라 줌
		EnhancedInputComponent->BindAction(IA_ZOOM_In, ETriggerEvent::Triggered, this, &AJSH_Player::Camera_Zoom_In);
		EnhancedInputComponent->BindAction(IA_ZOOM_Out, ETriggerEvent::Triggered, this, &AJSH_Player::Camera_Zoom_Out);
		EnhancedInputComponent->BindAction(IA_ZOOM_Default, ETriggerEvent::Started, this, &AJSH_Player::Camera_Zoom_Default);

		// 카메라 회전
		EnhancedInputComponent->BindAction(IA_Camera_Right, ETriggerEvent::Triggered, this, &AJSH_Player::CameraRight);
		EnhancedInputComponent->BindAction(IA_Camera_Left, ETriggerEvent::Triggered, this, &AJSH_Player::CameraLeft);
		EnhancedInputComponent->BindAction(IA_Camera_Default, ETriggerEvent::Started, this, &AJSH_Player::CameraDefault);

		// 마우스 감도
		EnhancedInputComponent->BindAction(IA_Mouse_Sensitive_Down, ETriggerEvent::Started, this, &AJSH_Player::Mouse_Sensitivity);
		EnhancedInputComponent->BindAction(IA_Mouse_Sensitive_Up, ETriggerEvent::Started, this, &AJSH_Player::Mouse_Sensitivity);


		// 세션 나가는
		EnhancedInputComponent->BindAction(IA_ESC, ETriggerEvent::Started, this, &AJSH_Player::Esc);


		EnhancedInputComponent->BindAction(IA_Del, ETriggerEvent::Started, this, &AJSH_Player::EditorAcotorDestroy);


		// Gizmo Click
		EnhancedInputComponent->BindAction(IA_Gizmo_Click, ETriggerEvent::Started, this, &AJSH_Player::Gizmo_Click);
		EnhancedInputComponent->BindAction(IA_Gizmo_Click, ETriggerEvent::Completed, this, &AJSH_Player::Gizmo_Click_End);


		// Gizmo Mode
		EnhancedInputComponent->BindAction(IA_Gizmo_SelectMode, ETriggerEvent::Started, this, &AJSH_Player::G_SelecteMode);
		EnhancedInputComponent->BindAction(IA_Gizmo_TranslateMode, ETriggerEvent::Started, this, &AJSH_Player::G_TranslateMode);
		EnhancedInputComponent->BindAction(IA_Gizmo_ScaleMode, ETriggerEvent::Started, this, &AJSH_Player::G_SclaeMode);
		EnhancedInputComponent->BindAction(IA_Gizmo_RotateMode, ETriggerEvent::Started, this, &AJSH_Player::G_RotateMode);

		EnhancedInputComponent->BindAction(IA_PreviousLocation, ETriggerEvent::Started, this, &AJSH_Player::Return_Previous_location);
	}
	else
	{
		//UE_LOG(LogTemplateCharacter, Error, TEXT("'%s"), *GetNameSafe(this));
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
		// * MouseSensitivity Yaw/Pitch 마우스 감도 조절
		AddControllerYawInput(LookAxisVector.X * MouseSensitivityYaw);
		AddControllerPitchInput(LookAxisVector.Y * MouseSensitivityPitch);
	}
}

#pragma endregion


#pragma region Editor (Player <-> SpectatorPawn)



// (F) 에디터 모드로 변환하는 함수
void AJSH_Player::SpectatorMode()
{
	// 메인 플랫폼 일떄 기능 LOCK
	if(!Bool_MainLock) return;
	
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
	
	
	// 3. 블루프린트 클래스 로드 -> Possess 바꿔주기 + Visible 끄기
	UClass* SpectatorClass = StaticLoadClass(AActor::StaticClass(), nullptr, TEXT("/Game/JSH/BP/BP_Spectator.BP_Spectator_C"));
    
	if (SpectatorClass)
	{
		//AActor* SpectatorActor = GetWorld()->SpawnActor<AActor>(SpectatorClass, CameraTransform, SpawnParams);
		//ASpectatorPawn* SpectatorActor = GetWorld()->SpawnActor<ASpectatorPawn>(tt->SpectatorClass, CameraTransform, SpawnParams);
		AActor* SpectatorActor = GetWorld()->SpawnActorDeferred<AActor>(SpectatorClass, CameraTransform);

		// 원랜 켜야하는데 beginplay에서 해주고 있어서 일단 주석 처리
		//AJSH_PlayerController* PlayerController = Cast<AJSH_PlayerController>(GetWorld()->GetFirstPlayerController());

		if (JPlayerController == nullptr) Saved_PlayerController();
		
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
	if(!Bool_MainLock) return;
	
	if(EditorMode_B) return;
	
	if (HasAuthority())
	{
		// FString RelativePath = FPaths::ProjectContentDir();
		//
		// FString FullPath = IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(*RelativePath);
		// IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(*RelativePath);
		// UE_LOG(LogTemp, Error, TEXT("RelativePath: %s"), *RelativePath);
		// UE_LOG(LogTemp, Error, TEXT("FullPath: %s"), *FullPath);
	
		// Record 함수를 끌고 오기 위한 GameInstance 
		ObsGamInstance = Cast<UJSH_OBSWebSocket>(GetGameInstance());
		CHJ_Instance = Cast<UMainGameInstance>(GetGameInstance());
		
		// 카메라 확대, 축소, 회전 , 초기화
		CameraReset();

		if (JPlayerController == nullptr) Saved_PlayerController();
		// 인스턴스에 넣어둔 녹화 기능 시작
		//ObsGamInstance->StartRecord();

		if (JPlayerController)
		{
			JPlayerController->StartRecord();
			NetMulti_StartRecording();
		}
	}

	// NetMulti_StartRecording();
}

void AJSH_Player::NetMulti_StartRecording_Implementation()
{
	if (!Record_b_On_Off)
	{
		if (!FlyMode_b_On_Off) FlyMode();
		// @@@캐릭터 없어짐 @@@@@
		// // 3인칭 -> 1인칭 변환
		// FollowCamera->SetActive(false);
		// RecordCamera->SetActive(true);
		// bUseControllerRotationYaw = true;
		// Camera_b_Third_First = true;
		//
		// // 카메라 소환
		// FallGuys_Camera->SetVisibility(true);
		// CameraSpawn_b_On_Off = true;
		// UE_LOG(LogTemp, Warning, TEXT("visible true"));

		FallGuys->SetCastShadow(false);
		Record_b_On_Off = true;
	}
	else
	{
		// @@@캐릭터 없어짐 @@@@@
		// // 1인칭 -> 3인칭 변환
		// RecordCamera->SetActive(false);
		// FollowCamera->SetActive(true);
		// // 비행 상태가 아닐때에만 Yaw를 꺼줌
		// if (!GetCharacterMovement()->IsFlying())
		// {
		// 	bUseControllerRotationYaw = false;
		// }
		// Camera_b_Third_First = false;
		//
		// // 카메라 제거
		// FallGuys_Camera->SetVisibility(false);
		// CameraSpawn_b_On_Off = false;

		FallGuys->SetCastShadow(true);
		Record_b_On_Off = false;


		// @@@캐릭터 없어짐 @@@@@
		// // Record Mode를 끌때에 아래로 레이 한번 쏴서 , 바닥에 가까우면 Fly 모드 종료
		// FVector Start = GetActorLocation();
		// FVector End = Start - FVector(0.f, 0.f, 1000.f);
		//
		// FHitResult HitResult;
		// FCollisionQueryParams Params;
		// Params.AddIgnoredActor(this);  // 자기 자신은 충돌 제외
		//
		// bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);
		//
		// if (bHit)
		// {
		// 	float DistanceToGround = HitResult.Distance;
		//
		// 	
		// 	if (DistanceToGround < 100.0f)
		// 	{
		// 		// 거리가 바닥이랑 가까울때 나는 모드면 , FLYMODE()를 가져와서 꺼주고
		// 		if (FlyMode_b_On_Off)
		// 		{
		// 			FlyMode();
		// 		}
		// 		else
		// 		{
		// 			GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		// 		}
		// 	}
		// 	
		// 	//DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 1, 0, 5);
		// }
		// else
		// {
		// 	
		// 	//DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 1, 0, 5);
		// }
	}
}


// 1인칭 <-> 3인칭 ,
void AJSH_Player::Camera_Third_First_Change()
{
	NetMulti_Camera_Third_First_Change();
}

void AJSH_Player::NetMulti_Camera_Third_First_Change_Implementation()
{
	// @@@ 캐릭터 없어짐 @@
	// if (!CameraSpawn_b_On_Off)
	// {
	// 	// 3인칭 -> 1인칭 변환
	// 	FollowCamera->SetActive(false);
	// 	RecordCamera->SetActive(true);
	// 	bUseControllerRotationYaw = true;
	// 	CameraSpawn_b_On_Off = true;
	// }
	// else
	// {
	// 	// 1인칭 -> 3인칭 변환
	// 	RecordCamera->SetActive(false);
	// 	FollowCamera->SetActive(true);
	// 	// 비행 상태가 아닐때에만 Yaw를 꺼줌
	// 	if (!GetCharacterMovement()->IsFlying())
	// 	{
	// 		bUseControllerRotationYaw = false;
	// 	}
	// 	CameraSpawn_b_On_Off = false;
	// }
	// Camera_b_Third_First = !Camera_b_Third_First;
}


// 카메라 스폰
void AJSH_Player::CameraSpawn()
{
	NetMulti_CameraSpawn();
}

void AJSH_Player::NetMulti_CameraSpawn_Implementation()
{
	// @@ 캐릭터 없어짐 @@
	// if (!CameraSpawn_b_On_Off)
	// {
	// 	FallGuys_Camera->SetVisibility(true);
	// }
	// else
	// {
	// 	FallGuys_Camera->SetVisibility(false);
	// }
	//
	// CameraSpawn_b_On_Off = !CameraSpawn_b_On_Off;
}


#pragma endregion


#pragma region FlyMode


void AJSH_Player::FlySpeed(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("hhhhhhhhhhhhhh"));
	// 마우스 우클릭을 누르고 있고(Bool_ZoomMode = true) and Editor Mode가 아니라면 속도를 움직이는게 아니라 카메라 줌인 줌 아웃을 컨트롤
	if (Bool_ZoomMode && !EditorMode_B) return;

	// FlyMode가 아니라면 속도 조절 안 해 !!!
	if (!GetCharacterMovement()->IsFlying()) return;



	float tempvalue = Value.Get<float>();

	MaxFlySpeed_C = MaxFlySpeed_C + tempvalue * 50;
	
	UE_LOG(LogTemp, Warning, TEXT(" 마우스 휠: %f"), tempvalue);
	UE_LOG(LogTemp, Warning, TEXT(" MaxSpeed: %f"), MaxFlySpeed_C);

	GetCharacterMovement()->MaxFlySpeed = MaxFlySpeed_C;
}



void AJSH_Player::FlyMode()
{
	// 메인 플랫폼 일떄 기능 LOCK
	//if(!Bool_MainLock) return;
	
	NetMulti_FlyMode();


}
void AJSH_Player::NetMulti_FlyMode_Implementation()
{
	GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	bUseControllerRotationPitch = true;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = true;
	
	// if(!FlyMode_b_On_Off)
	// {
	// 	GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	// 	bUseControllerRotationPitch = true;
	// 	bUseControllerRotationYaw = true;
	// 	bUseControllerRotationRoll = true;
	// }
	// @@ 캐릭터 없어짐 @@@
	// else
	// {
	// 	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	// 	bUseControllerRotationPitch = false;
	// 	bUseControllerRotationYaw = false;
	// 	bUseControllerRotationRoll = false;
	// }
	// FlyMode_b_On_Off = !FlyMode_b_On_Off;
}


// (E) 위로 올라가는 - 레이 안 쏨
void AJSH_Player::Fly_Up_Down(const FInputActionValue& Value)
{
	NetMulti_Fly_Up_Down(Value);
}

void AJSH_Player::NetMulti_Fly_Up_Down_Implementation(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("오잉"))
	// @@ 캐릭터 없어짐 @@
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
	// if (!FlyMode_b_On_Off)
	// {
	// 	FlyMode();	
	// }
	if (GetCharacterMovement()->IsFlying())
	{
		UE_LOG(LogTemp, Warning, TEXT("f111"))
	}
	
	if (GetCharacterMovement()->IsFlying())
	{
		UE_LOG(LogTemp, Warning, TEXT("f222"))
		if (!DisableEdit_b) return;

		// 입력 값에서 Up/Down 액션 값 추출
		Fly_Zvalue = Value.Get<float>();
		UE_LOG(LogTemp, Warning, TEXT("처음: %f"), Fly_Zvalue)
		AddMovementInput(FVector(0.f, 0.f, 1.f), Fly_Zvalue);
	}
}


// (Q) 아래로 내려가는 -> 내려갈때 레이쏴서 일정 거리 가까워지면 FlyMode 종료 
void AJSH_Player::Fly_Down_Ray(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("down"));
	NetMulti_Fly_Down_Ray(Value);
}



void AJSH_Player::NetMulti_Fly_Down_Ray_Implementation(const FInputActionValue& Value)
{
	
	if (GetCharacterMovement()->IsFlying())
	{
		
		// 입력 값에서 Up/Down 액션 값 추출
		Fly_Zvalue = Value.Get<float>();
		AddMovementInput(FVector(0.f, 0.f, 1.f), Fly_Zvalue);

		//@@ 캐릭터 없어짐 @@@
		// // 아래에 있는 물체와의 거리 체크
		// FVector Start = GetActorLocation();
		// FVector End = Start - FVector(0.f, 0.f, 1000.f);
		
		//@@ 캐릭터 없어짐 @@@
		// FHitResult HitResult;
		// FCollisionQueryParams Params;
		// Params.AddIgnoredActor(this);  // 자기 자신은 충돌 제외
		//
		// bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);

		



		// @@ 캐릭터 없어짐 @@
		// if (bHit)
		// {
		// 	float DistanceToGround = HitResult.Distance;
		//
		// 	// Editor 모드가 아닐때 or Record Mode가 아닐때 Fly Mode를 종료
		// 	if (!EditorMode_B & !Record_b_On_Off)
		// 	{
		// 		// 원하는 거리 임계값 설정
		// 		if (DistanceToGround < 100.0f)
		// 		{
		// 			FlyMode(); 
		// 		}
		// 	}
		// 	
		// 	
		// 	//DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 1, 0, 5);
		// }
		// else
		// {
		// 	
		// 	//DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 1, 0, 5);
		// }
	}

}

#pragma endregion


#pragma region EditorMode


// (key: 1) EditorMode On / Off
void AJSH_Player::EditorMode()
{
	// 메인 플랫폼 일떄 기능 LOCK
	if(!Bool_MainLock) return;
	
	// 녹화 중이 아닐때에만 Editor 모드 가능
	if (Record_b_On_Off) return;

	// 클릭 중에 q나 tap누르면 튕기는 오류 때문에
	if(Gizmo_Clicking_forError) return;
	
	
	NetMulti_EditorMode();

	// if (!EditorMode_B)
	// {
	// 	// proto시연때 막아서
	// 	EnableEdit();
	// }
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

		// 기즈모 모드 임시
		if (!FirstGizmode)
		{
			Gizmo_TranslateMode = true;
			Gizmo_ScaleMode = false;
			Gizmo_RotateMode = false;

			FirstGizmode = true;
		}
		else
		{
			
		}


		// @@@캐릭터 없어짐 @@@@@
		// // 3인칭 -> 1인칭 변환
		// FollowCamera->SetActive(false);
		// RecordCamera->SetActive(true);
		// bUseControllerRotationYaw = true;
		// Camera_b_Third_First = true;

		// E를 누르지 않아도 임시 Fly Mode 
		// GetCharacterMovement()->SetMovementMode(MOVE_Flying);
		// bUseControllerRotationPitch = true;
		// bUseControllerRotationYaw = true;
		// bUseControllerRotationRoll = true;

		// proto시연때 막아서
		if (HasAuthority())
		{
			EnableEdit();
		}
	}
	// Editor Mode Off
	else
	{
		UE_LOG(LogTemp, Error, TEXT("EditorMode Off"));
		DisableEdit();
		
		// FlyMode를 제어하는 bool 값 (Editor Mode 일때 항상 날아다니 도록)
		EditorMode_B = false;

		// Gizmo 강제종료 문제
		if (Editor_SpawnActor != nullptr)
		{
			Editor_SpawnActor->OriginGizmo->Destroy();
			Editor_SpawnActor = nullptr;
			
		}
		
		
		// @@@캐릭터 없어짐 @@@@@
		// // 1인칭 -> 3인칭 변환
		// RecordCamera->SetActive(false);
		// FollowCamera->SetActive(true);
		// // 비행 상태가 아닐때에만 Yaw를 꺼줌
		// if (!GetCharacterMovement()->IsFlying())
		// {
		// 	bUseControllerRotationYaw = false;
		// 	bUseControllerRotationPitch = false;
		// 	bUseControllerRotationRoll = false;
		// }
		Camera_b_Third_First = false;


		// Editor 모드 종료 시 저장된 EditorSpwanAcotr Name 삭제
		// JPlayerController->Editor_SpawnActor = nullptr;
		Editor_SpawnActor = nullptr; // 에디터 모드가 아닐떄 삭제 못하게
		

		// @@@캐릭터 없어짐 @@@@@
		// Fly Mode를 끌때에 아래로 레이 한번 쏴서 , Fly 모드 종료
		// FVector Start = GetActorLocation();
		// FVector End = Start - FVector(0.f, 0.f, 1000.f);
		//
		// FHitResult HitResult;
		// FCollisionQueryParams Params;
		// Params.AddIgnoredActor(this);  // 자기 자신은 충돌 제외
		//
		// bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);
		//
		// if (bHit)
		// {
		// 	float DistanceToGround = HitResult.Distance;
		//
		// 	
		// 	if (DistanceToGround < 100.0f)
		// 	{
		// 		// 거리가 바닥이랑 가까울때 나는 모드면 , FLYMODE()를 가져와서 꺼주고
		// 		if (FlyMode_b_On_Off)
		// 		{
		// 			FlyMode();
		// 		}
		// 		// Fly Mode가 아닌데 바닥이랑 가까우면 요렇게 꺼줌
		// 		else
		// 		{
		// 			GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		// 		}
		// 	}
		// 	
		// 	//DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 1, 0, 5);
		// }
		// else
		// {
		// 	
		// 	//DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 1, 0, 5);
		// }
	}
}


void AJSH_Player::EnableEdit()
{
	// Editor 모드에서 마우스를 누른 상태로 , Editor 모드를 껏을때 , 꺼지고 나서 마우스를 떼면 EnableEdit가 On되는 문제가 있어
	// 이렇게 막아줌
	if (!EditorMode_B) return;
	
	DisableEdit_b = false;
	DisableEdit2_b = false;
	
	// 기즈모 Tick 제어를 위한 (Enable Edit 상태일 때 Ray 쏘도록)
	EnableEditSystem = true;
	
	// 마우스 우클릭을 안 하고 있으면 , ZoomMode를 Flase로
	Bool_ZoomMode = false;
	UE_LOG(LogTemp, Error, TEXT("false"));
	
	// // Editor 모드에서 마우스를 누른 상태로 , Editor 모드를 껏을때 , 꺼지고 나서 마우스를 떼면 EnableEdit가 On되는 문제가 있어
	// // 이렇게 막아줌
	// if (!EditorMode_B) return;


	
	GetMovementComponent()->SetComponentTickEnabled(false);
	if (JPlayerController == nullptr) Saved_PlayerController();
	
	if (JPlayerController)
	{
		JPlayerController->SetIgnoreLookInput(true);
		
		// 마우스 커서 보이게 설정
		JPlayerController->bShowMouseCursor = true;
		JPlayerController->bEnableClickEvents = true;
		JPlayerController->bEnableMouseOverEvents = true;
		
		// 마우스를 UI와 게임에서 사용할 수 있도록 설정
		// JPlayerController->SetInputMode(FInputModeGameAndUI());
		// GEngine->GameViewport->SetMouseLockMode(EMouseLockMode::LockAlways);
		
		FInputModeGameAndUI InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
		InputMode.SetHideCursorDuringCapture(false);
		JPlayerController->SetInputMode(InputMode);
	}

	if (IsLocallyControlled() && !PlayerMainUI)
	{
		if (UI_Editor_Main)
		{
			PlayerMainUI = CreateWidget<UUserWidget>(GetWorld(), UI_Editor_Main);
			if(PlayerMainUI)
			{
				PlayerMainUI->AddToViewport(0);
			}
		}
	}
}

void AJSH_Player::DisableEdit()
{
	if (!EditorMode_B) return;

	DisableEdit_b = true;
	DisableEdit2_b = true;
	
	// 기즈모 Tick 제어를 위한 (Enable Edit 상태일 때 Ray 쏘도록)
	EnableEditSystem = false;
	
	// 마우스 우클릭 하고 있으면, ZoomMode를 True로
	if (!Bool_ZoomMode)
	{
		Bool_ZoomMode = true;
		UE_LOG(LogTemp, Error, TEXT("true"));
	}
	
	GetMovementComponent()->SetComponentTickEnabled(true);

	if (JPlayerController == nullptr) Saved_PlayerController();
	if (JPlayerController)
	{
		JPlayerController->SetIgnoreLookInput(false);
		
		JPlayerController->bShowMouseCursor = false;
		JPlayerController->bEnableClickEvents = false;
		JPlayerController->bEnableMouseOverEvents = false;
		JPlayerController->SetInputMode(FInputModeGameOnly());
	}

		JPlayerController = Cast<AJSH_PlayerController>(GetWorld()->GetFirstPlayerController());

	if (PlayerMainUI)
	{
		PlayerMainUI->RemoveFromParent();
		PlayerMainUI = nullptr;  // 포인터를 null로 설정
		UE_LOG(LogTemp, Warning, TEXT("UI null"));
	}
}

void AJSH_Player::CLickAndDel()
{
	// 클릭 하고 DelButton 눌러야 삭제 되도록
	ClickedEditorActor = !ClickedEditorActor;
}


// EditorActor를 클릭하면 그곳에서 자기 정보를 SaveEditorActor(AJSH_Editor_SpawnActor* ClickedActor) 여기로 전달 후 저장
void AJSH_Player::SaveEditorActor(AJSH_Editor_SpawnActor* ClickedActor)
{
	NetMulti_SaveEditorActor_Implementation(ClickedActor);
}

void AJSH_Player::NetMulti_SaveEditorActor_Implementation(AJSH_Editor_SpawnActor* ClickedActor)
{
	Editor_SpawnActor = ClickedActor;
	UE_LOG(LogTemp, Error, TEXT("ss 111"));
	// 이전 위치 돌아가는 함수 
	if (Recent_Clicked_SpawnActor == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("ss 22"));
		Recent_Clicked_SpawnActor = ClickedActor;
		//Recent_Clicked_SpawnActor->Onclicked();
	}
	else if (Recent_Clicked_SpawnActor != nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("ss 33"));
		First_Clicked_SpawnActor = Recent_Clicked_SpawnActor;
		First_Clicked_SpawnActor->Unclicked();
		Recent_Clicked_SpawnActor = ClickedActor;
		//Recent_Clicked_SpawnActor->Onclicked();
		
	}
}



#pragma endregion


#pragma region Editor_Gizmo



// Gizmo 정보 저장 ///

void AJSH_Player::Save_Gizmo_Parents(AActor* Gizmo_Parents)
{
	Saved_Gizmo_Parents = Cast<AJSH_Gizmo>(Gizmo_Parents);
}

// Translate 정보 저장 

void AJSH_Player::Save_Gizmo_TX(AActor* Gizmo_TX)
{
	Saved_Gizmo_TX = Cast<AJSH_Translate_GizmoX>(Gizmo_TX);
}
void AJSH_Player::Save_Gizmo_TY(AActor* Gizmo_TY)
{
	Saved_Gizmo_TY = Cast<AJSH_Translate_GizmoY>(Gizmo_TY);
}
void AJSH_Player::Save_Gizmo_TZ(AActor* Gizmo_TZ)
{
	Saved_Gizmo_TZ = Cast<AJSH_Translate_GizmoZ>(Gizmo_TZ);
}
void AJSH_Player::Save_Gizmo_TB(AActor* Gizmo_TB)
{
	Saved_Gizmo_TB = Cast<AJSH_Translate_GizmoBox>(Gizmo_TB);
}

// Scale정보 저장 

void AJSH_Player::Save_Scale_SX(AActor* Gizmo_SX)
{
	Saved_Gizmo_SX = Cast<AJSH_Scale_GizmoX>(Gizmo_SX);
}
void AJSH_Player::Save_Scale_SY(AActor* Gizmo_SY)
{
	Saved_Gizmo_SY = Cast<AJSH_Scale_GizmoY>(Gizmo_SY);
}
void AJSH_Player::Save_Scale_SZ(AActor* Gizmo_SZ)
{
	Saved_Gizmo_SZ = Cast<AJSH_Scale_GizmoZ>(Gizmo_SZ);
}








void AJSH_Player::EditorAcotorDestroy()
{
	NetMulti_EditorAcotorDestroy();
}
void AJSH_Player::NetMulti_EditorAcotorDestroy_Implementation()
{
	if (Editor_SpawnActor != nullptr)
	{
		Editor_SpawnActor->DestroyThis();
	}
}


void AJSH_Player::G_SelecteMode()
{
	if (!EditorMode_B) return;
	if (DisableEdit_b) return;
	// 클릭 중에 q나 tap누르면 튕기는 오류 때문에 + 잡고 있을 떄 누르면 모드 바껴도 원래 상태로 
	if (Gizmo_Clicking_forError) return;

	// 여기는 UI 바뀌는거 넣음 될 듯
	
	if (Editor_SpawnActor == nullptr) return;
	if (Saved_Gizmo_TX == nullptr || Saved_Gizmo_TY == nullptr || Saved_Gizmo_TZ == nullptr || Saved_Gizmo_TB == nullptr) return;

	
	UE_LOG(LogTemp, Warning, TEXT("g select"));
	Editor_SpawnActor = nullptr;
	
	

	
	Saved_Gizmo_TX->Visible_and_Collision_Off();
	Saved_Gizmo_TY->Visible_and_Collision_Off();
	Saved_Gizmo_TZ->Visible_and_Collision_Off();
	Saved_Gizmo_TB->Visible_and_Collision_Off();
	
}

void AJSH_Player::G_TranslateMode()
{
	if (!EditorMode_B) return;
	if (DisableEdit_b) return;
	// 클릭 중에 q나 tap누르면 튕기는 오류 때문에 + 잡고 있을 떄 누르면 모드 바껴도 원래 상태로 
	if (Gizmo_Clicking_forError) return;

	// 여기는 UI 바뀌는거 넣음 될 듯
	
	if (Editor_SpawnActor == nullptr) return;
	if (Saved_Gizmo_TX == nullptr || Saved_Gizmo_TY == nullptr || Saved_Gizmo_TZ == nullptr || Saved_Gizmo_TB == nullptr) return;
	UE_LOG(LogTemp, Warning, TEXT("g translate"));
	Gizmo_TranslateMode = true;
	Gizmo_RotateMode = false;
	Gizmo_ScaleMode = false;

	Saved_Gizmo_TX->Visible_and_Collision_On();
	Saved_Gizmo_TY->Visible_and_Collision_On();
	Saved_Gizmo_TZ->Visible_and_Collision_On();
	Saved_Gizmo_TB->Visible_and_Collision_On();
}


void AJSH_Player::G_RotateMode()
{
	if (!EditorMode_B) return;
	if (DisableEdit_b) return;
	// 클릭 중에 q나 tap누르면 튕기는 오류 때문에 + 잡고 있을 떄 누르면 모드 바껴도 원래 상태로 
	if (Gizmo_Clicking_forError) return;

	// 여기는 UI 바뀌는거 넣음 될 듯
	
	if (Editor_SpawnActor == nullptr) return;
	if (Saved_Gizmo_TX == nullptr || Saved_Gizmo_TY == nullptr || Saved_Gizmo_TZ == nullptr || Saved_Gizmo_TB == nullptr) return;
	UE_LOG(LogTemp, Warning, TEXT("g  rotate"));
	Gizmo_TranslateMode = false;
	Gizmo_RotateMode = true;
	Gizmo_ScaleMode = false;

	Saved_Gizmo_TX->Visible_and_Collision_Off();
	Saved_Gizmo_TY->Visible_and_Collision_Off();
	Saved_Gizmo_TZ->Visible_and_Collision_Off();
	Saved_Gizmo_TB->Visible_and_Collision_Off();
}


void AJSH_Player::G_SclaeMode()
{
	if (!EditorMode_B) return;
	if (DisableEdit_b) return;
	// 클릭 중에 q나 tap누르면 튕기는 오류 때문에 + 잡고 있을 떄 누르면 모드 바껴도 원래 상태로 
	if (Gizmo_Clicking_forError) return;

	// 여기는 UI 바뀌는거 넣음 될 듯 
	
	if (Editor_SpawnActor == nullptr) return;
	if (Saved_Gizmo_TX == nullptr || Saved_Gizmo_TY == nullptr || Saved_Gizmo_TZ == nullptr || Saved_Gizmo_TB == nullptr) return;
	UE_LOG(LogTemp, Warning, TEXT("g scale"));
	Gizmo_TranslateMode = false;
	Gizmo_RotateMode = false;
	Gizmo_ScaleMode = true;

	Saved_Gizmo_TX->Visible_and_Collision_Off();
	Saved_Gizmo_TY->Visible_and_Collision_Off();
	Saved_Gizmo_TZ->Visible_and_Collision_Off();
	Saved_Gizmo_TB->Visible_and_Collision_Off();
}


void AJSH_Player::Gizmo_Detect()
{
	if (!EditorMode_B) return;
	if (DisableEdit_b) return;
	if (Saved_Gizmo_TX == nullptr || Saved_Gizmo_TY == nullptr || Saved_Gizmo_TZ == nullptr || Saved_Gizmo_TB == nullptr) return;
	
	if (JPlayerController == nullptr) Saved_PlayerController();
	//// 마우스 2d Vector -> 3d Vector ////
	if (JPlayerController->GetMousePosition(MousePosition.X, MousePosition.Y))
	{
		JPlayerController->DeprojectMousePositionToWorld(Mouse_WorldLocation, Mouse_WorldDirection);
	}
	
	///// Ray ////
	Start = Mouse_WorldLocation;
	End = (Mouse_WorldDirection * 10000.0f) + Mouse_WorldLocation;
	
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_GameTraceChannel2, Params);

	if (bHit)
	{
		// 기즈모가 감지 되었을떄 true -> Gizmo Click이 가능해짐.
		
		if (Gizmo_Detecting == false) Gizmo_Detecting = true;
		
		//DrawDebugLine(GetWorld(), Start, End, FColor::Purple, false, 10, 0, 0.1);
		if (Saved_Gizmo_TX != nullptr)
		{
			if (HitResult.GetActor())
			{
				Saved_Gizmo_TX->BeginCursorOver();

				Saved_Gizmo_TY->EndCursorOver();
				Saved_Gizmo_TZ->EndCursorOver();
				Saved_Gizmo_TB->EndCursorOver();
			}
		}
		if (Saved_Gizmo_TY != nullptr)
		{
			if (HitResult.GetActor() == Saved_Gizmo_TY)
			{
				Saved_Gizmo_TY->BeginCursorOver();

				Saved_Gizmo_TX->EndCursorOver();
				Saved_Gizmo_TZ->EndCursorOver();
				Saved_Gizmo_TB->EndCursorOver();
			}
		}
		if (Saved_Gizmo_TZ != nullptr)
		{
			if (HitResult.GetActor() == Saved_Gizmo_TZ)
			{
				Saved_Gizmo_TZ->BeginCursorOver();

				Saved_Gizmo_TY->EndCursorOver();
				Saved_Gizmo_TX->EndCursorOver();
				Saved_Gizmo_TB->EndCursorOver();
			}
		}
		if (Saved_Gizmo_TB != nullptr)
		{
			if (HitResult.GetActor() == Saved_Gizmo_TB)
			{
				Saved_Gizmo_TB->BeginCursorOver();

				Saved_Gizmo_TY->EndCursorOver();
				Saved_Gizmo_TX->EndCursorOver();
				Saved_Gizmo_TZ->EndCursorOver();
			}
		}
	}
	else
	{
		if (Gizmo_Detecting == true) Gizmo_Detecting = false;
		
		HitResult.Reset();
		
		if (Saved_Gizmo_TX != nullptr)
		{
			Saved_Gizmo_TX->EndCursorOver();
		}
		if (Saved_Gizmo_TY != nullptr)
		{
			Saved_Gizmo_TY->EndCursorOver();
		}
		if (Saved_Gizmo_TZ != nullptr)
		{
			Saved_Gizmo_TZ->EndCursorOver();
		}
		if (Saved_Gizmo_TB != nullptr)
		{
			Saved_Gizmo_TB->EndCursorOver();
		}
	}
}


void AJSH_Player::Gizmo_Click()
{

	if (!EditorMode_B) return;
	if (DisableEdit_b) return;
	
	// 기즈모가 감지되어 있지 않다면 , 클릭 x
	if (Gizmo_Detecting == false) return;

	// 클릭 중에 q나 tap누르면 튕기는 오류 때문에
	Gizmo_Clicking_forError = true;
	
	if (Saved_Gizmo_TX != nullptr)
	{
		if (HitResult.GetActor() == Saved_Gizmo_TX)
		{
			Saved_Gizmo_TX->GOnClicked();
			Clicked_TX = true;
		}
	}
	
	if (Saved_Gizmo_TY != nullptr)
	{
		if (HitResult.GetActor() == Saved_Gizmo_TY)
		{
			Saved_Gizmo_TY->GOnClicked();
			Clicked_TY = true;
		}
	}

	if (Saved_Gizmo_TZ != nullptr)
	{
		if (HitResult.GetActor() == Saved_Gizmo_TZ)
		{
			Saved_Gizmo_TZ->GOnClicked();
			Clicked_TZ = true;
		}
	}
	
	if (Saved_Gizmo_TB != nullptr)
	{
		if (HitResult.GetActor() == Saved_Gizmo_TB)
		{
			Saved_Gizmo_TB->GOnClicked();
			Clicked_TB = true;
		}
	}
	
	
	// Last Location 저장
	if (Editor_SpawnActor != nullptr)
	{
		Editor_SpawnActor->AddPreviousLocation(Editor_SpawnActor->GetActorLocation());
		//Previous_Click_Actor = nullptr;
	}
}

void AJSH_Player::Gizmo_Click_End()
{
	if (!Gizmo_Clicking_forError) return;

	if (Clicked_TX)
	{
		Saved_Gizmo_TX->HandleMouseReleaseOutsideActor();
	}
	if (Clicked_TY)
	{
		Saved_Gizmo_TY->HandleMouseReleaseOutsideActor();
	}
	if (Clicked_TZ)
	{
		Saved_Gizmo_TZ->HandleMouseReleaseOutsideActor();
	}
	if (Clicked_TB)
	{
		Saved_Gizmo_TB->HandleMouseReleaseOutsideActor();
	}

	// 클릭 중에 q나 tap누르면 튕기는 오류 때문에
	Gizmo_Clicking_forError = false;
}


void AJSH_Player::AddPreviousLocation(const FVector& newLocation)
{
	
	if (PreviousLocations.size() >= MaxLocations)
	{
		PreviousLocations.pop();  // 제일 오래된 위치 제거
	}
	PreviousLocations.push(newLocation);
}


void AJSH_Player::Return_Previous_location()
{
	if (!EditorMode_B) return;
	if (DisableEdit_b) return;
	
	// 클릭 중에 q나 tap누르면 튕기는 오류 때문에 + 잡고 있을 떄 누르면 모드 바껴도 원래 상태로 
	if (Gizmo_Clicking_forError) return;

	// 여기는 UI 바뀌는거 넣음 될 듯
	if (Editor_SpawnActor == nullptr) return;
	////////////////////////////////////////////////////////////////////////////////

	
	if (Now_Click_Actor != nullptr && Now_Click_Actor->PreviousLocations.size() >= 1)
	{
		Now_Click_Actor->ReturnPreviousLocation();
		UE_LOG(LogTemp, Error, TEXT("~ 1"));
	}
	else if (Now_Click_Actor->PreviousLocations.size() == 0 && Previous_Click_Actor != nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("~ 2-1"));
		SaveEditorActor(Previous_Click_Actor);
		//Previous_Click_Actor->GizmoSpawn();
		Previous_Click_Actor = nullptr;
		UE_LOG(LogTemp, Error, TEXT("~ 2-2"));
	}
	else
	{
		if (Editor_SpawnActor != nullptr)
		{
			Editor_SpawnActor->ReturnPreviousLocation();
			UE_LOG(LogTemp, Error, TEXT("~ 3"));
		}
	}

	
	// 값 초기화는 Editor Spawn Actor에서 해주고 있음 (새로 다른 actor 클릭했을 시)
}




#pragma endregion


#pragma region Camera Control




void AJSH_Player::Camera_Zoom_In()
{
	// 마우스 우클릭을 누르고 있고(Bool_ZoomMode = true) and Editor Mode가 아니라면 속도를 움직이는게 아니라 카메라 줌인 줌 아웃을 컨트롤
	// if (Bool_ZoomMode && !EditorMode_B)

	// @@ 캐릭터 없어짐 @@
	// Recording 중이 아니라면 실행 하지 않음
	//if (!Record_b_On_Off) return;

	if (!EditorMode_B)
	{
		RecordCamera->FieldOfView -= ZoomSpeed;
		
		if (RecordCamera->FieldOfView  <= 10.0f)
		{
			RecordCamera->FieldOfView  = 10.0f;
			RecordCamera->SetFieldOfView(RecordCamera->FieldOfView);
		}
		else
		{
			RecordCamera->SetFieldOfView(RecordCamera->FieldOfView );
		}
	}
}

void AJSH_Player::Camera_Zoom_Out()
{
	// 마우스 우클릭을 누르고 있고(Bool_ZoomMode = true) and Editor Mode가 아니라면 속도를 움직이는게 아니라 카메라 줌인 줌 아웃을 컨트롤
	// if (Bool_ZoomMode && !EditorMode_B)

	// @@ 캐릭터 없어짐 @@
	// Recording 중이 아니라면 실행 하지 않음
	// if (!Record_b_On_Off) return;
	
	if (!EditorMode_B)
	{
		RecordCamera->FieldOfView += ZoomSpeed;
		 if (RecordCamera->FieldOfView  >= 120.0f)
		 {
		 	RecordCamera->FieldOfView  = 120.0f;
		 	RecordCamera->SetFieldOfView(RecordCamera->FieldOfView );
		 }
		 else
		 {
		 	RecordCamera->SetFieldOfView(RecordCamera->FieldOfView );
		 }
	}
}

void AJSH_Player::Camera_Zoom_Default()
{
	if (EditorMode_B) return;
	
		
	RecordCamera->SetFieldOfView(90.0f);
}


void AJSH_Player::CameraRight()
{
	// @@ 캐릭터 없어짐 @@
	// 1인칭이 아니라면 실행 하지 않음
	//if (!RecordCamera->IsActive()) return;
	// Recording 중이 아니라면 실행 하지 않음
	//if (!Record_b_On_Off) return;
	
	CurrentAngl += RotateSpeed;
	
	// if (CurrentAngl > 90.0f) 
	// {
	// 	CurrentAngl = 90.0f;
	// }
	
	NewCameraRotation = RecordCamera->GetRelativeRotation();
	NewCameraRotation.Roll = CurrentAngl;
	RecordCamera->SetRelativeRotation(NewCameraRotation);
}

void AJSH_Player::CameraLeft()
{
	// @@ 캐릭터 없어짐 @@
	// 1인칭이 아니라면 실행 하지 않음
	// if (!RecordCamera->IsActive()) return;
	// Recording 중이 아니라면 실행 하지 않음
	// if (!Record_b_On_Off) return;
	
	CurrentAngl -= RotateSpeed;
	
	// if (CurrentAngl < -90.0f) 
	// {
	// 	CurrentAngl = -90.0f;
	// }
	
	NewCameraRotation = RecordCamera->GetRelativeRotation();
	NewCameraRotation.Roll = CurrentAngl;
	RecordCamera->SetRelativeRotation(NewCameraRotation);
}

void AJSH_Player::CameraDefault()
{
	// @@ 캐릭터 없어짐 @@
	// // 1인칭이 아니라면 실행 하지 않음
	// if (!RecordCamera->IsActive()) return;
	// // Recording 중이 아니라면 실행 하지 않음
	// if (!Record_b_On_Off) return;
	
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

// 마우스 감도 조절 , 줌 했을때 마우스(화면 회전)가 너무 빨라서 추가
void AJSH_Player::Mouse_Sensitivity(const FInputActionValue& Value)
{
	float Temp_FG = Value.Get<float>();

	// UE_LOG(LogTemp, Error, TEXT("tttt %f"), Temp_FG);

	
	if (Temp_FG >= +1)
	{
		MouseSensitivityYaw += 0.05;
		MouseSensitivityPitch += 0.05;

		// 1도 완저ㅓㅓㅓㅓ언 빠라서 굳이 1 이상으로 갈 필요가 있을까 싶음
		if (MouseSensitivityYaw >= 1.0)
		{
			MouseSensitivityYaw = 1.0;
			MouseSensitivityPitch = 1.0;
		}
	}
	else
	{
		MouseSensitivityYaw -= 0.05;
		MouseSensitivityPitch -= 0.05;

		// - 값으로 가면 마우스 움직임이 아예 반대로 들어가서 막았으
		if (MouseSensitivityYaw <= 0.05)
		{
			MouseSensitivityYaw = 0.05;
			MouseSensitivityPitch = 0.05;
		}
	}
	
	UE_LOG(LogTemp, Error, TEXT("YYY %f"), MouseSensitivityYaw);
	UE_LOG(LogTemp, Error, TEXT("PPP %f"), MouseSensitivityPitch);
}



#pragma endregion


#pragma region Session

void AJSH_Player::Esc()
{
	// Editor 모드일때 ESC 누르면 선택 없어지는거
	if (EditorMode_B == true)
	{
		Editor_SpawnActor = nullptr;
		
	}
	// Editor 모드아닐때 ESC 방 나가기
	else
	{
		// 종료하겠습니까 위젯 하나 뛰어줘야할듯
		CHJ_Instance->ExitSession();
		UE_LOG(LogTemp, Error, TEXT("ESC"));
	}
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