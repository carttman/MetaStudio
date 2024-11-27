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
#include <string>

#include "JSH_Editor_SpawnActor.h"
#include "JSH_PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
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
#include "Gizmo/JSH_Rotate_GizmoZ.h"
#include "Gizmo/JSH_Scale_GizmoBox.h"
#include "Gizmo/JSH_Scale_GizmoX.h"
#include "Gizmo/JSH_Scale_GizmoY.h"
#include "Gizmo/JSH_Scale_GizmoZ.h"
#include "Gizmo/JSH_Translate_GizmoBox.h"
#include "Gizmo/JSH_Translate_GizmoX.h"
#include "Gizmo/JSH_Translate_GizmoY.h"
#include "Gizmo/JSH_Translate_GizmoZ.h"
#include "MetaStudios/CHJ/MainGameInstance.h"
#include "MetaStudios/CHJ/Component/FirebaseComponent.h"
#include "Net/UnrealNetwork.h"
#include "UObject/ConstructorHelpers.h"
#include "UObject/UObjectGlobals.h"
#include "Widget/JSH_Record_UI.h"


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
	GetCharacterMovement()->GravityScale = 0.0;
	

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


	// FallGuys = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Body"));
	// FallGuys->SetupAttachment(RootComponent);
	// ConstructorHelpers::FObjectFinder<USkeletalMesh> TMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/JSH/Asset/Corgi/untitled.untitled'"));
	// if (TMesh.Succeeded())
	// {
	// 	FallGuys->SetSkeletalMesh(TMesh.Object);
	// 	FallGuys->SetRelativeLocationAndRotation(FVector(20, 0, -12), FRotator(0, -90, 0));
	// 	FallGuys->SetRelativeScale3D(FVector(0.5, 0.5, 0.5));
	// 	FallGuys->SetCastShadow(true);
	// 	
	// 	//FallGuys->SetVisibility(false);
	// }
	

	// FallGuys_Camera = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Record Camera"));
	// FallGuys_Camera->SetupAttachment(FallGuys);
	// ConstructorHelpers::FObjectFinder<USkeletalMesh> TMesh2(TEXT("/Script/Engine.SkeletalMesh'/Game/JSH/Asset/Camera03/source/video-cam.video-cam'"));
	// if (TMesh2.Succeeded())
	// {
	// 	FallGuys_Camera->SetSkeletalMesh(TMesh2.Object);
	// 	FallGuys_Camera->SetRelativeLocationAndRotation(FVector(-12477.217394, 3931.275206, 24551.795870), FRotator(1.727941, 0.148925, 9.851076));
	// 	FallGuys_Camera->SetRelativeScale3D(FVector(100, 100, 100));
	//
	// 	FallGuys->SetVisibility(false);
	// }

	
	//GetCharacterMovement()->MaxFlySpeed = MaxFlySpeed_C;
	GetCharacterMovement()->MaxFlySpeed = MaxFlySpeed_C;
	GetCharacterMovement()->BrakingDecelerationFlying = BrakingDecelerationFlying_C;

	FirebaseComponent = CreateDefaultSubobject<UFirebaseComponent>(TEXT("FirebaseComponent"));



	Root_Camera01 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Root_Camera01"));
	Root_Camera01->SetupAttachment(RootComponent);
	Root_Camera01->SetRelativeLocation(FVector(70.f, -74.f, -265.f));
	Root_Camera01->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));
	Root_Camera01->SetRelativeScale3D(FVector(1.5f, 1.5f, 1.5f));
	Root_Camera01->SetCastShadow(false);
	
	Camera02 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Camera02"));
	Camera02->SetupAttachment(Root_Camera01);
	Camera02->SetCastShadow(false);
	
	Camera03 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Camera03"));
	Camera03->SetupAttachment(Root_Camera01);
	Camera03->SetCastShadow(false);
	
	Camera04 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Camera04"));
	Camera04->SetupAttachment(Root_Camera01);
	Camera04->SetCastShadow(false);
	
	Camera05 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Camera05"));
	Camera05->SetupAttachment(Root_Camera01);
	Camera05->SetCastShadow(false);
	
	Camera06 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Camera06"));
	Camera06->SetupAttachment(Root_Camera01);
	Camera06->SetCastShadow(false);

	Camera07 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Camera07"));
	Camera07->SetupAttachment(Root_Camera01);
	Camera07->SetCastShadow(false);

	
}
#pragma endregion


#pragma region Begin, Multi, Input

void AJSH_Player::BeginPlay()
{
	Super::BeginPlay();
	

	FlyMode();
	
	// 캐릭터 없어져서 그냥 바로 1인칭 시작
	FollowCamera->SetActive(false);
	RecordCamera->SetActive(true);
	bUseControllerRotationYaw = true;
	CameraSpawn_b_On_Off = true;
	
	
	if (IsLocallyControlled() && !RecordUI_01)
	{
		if (UI_Record_01)
		{
			RecordUI_01 = CreateWidget<UUserWidget>(GetWorld(), UI_Record_01);
			if(RecordUI_01)
			{
				RecordUI_01->AddToViewport();
				Origin_RecordUI = Cast<UJSH_Record_UI>(RecordUI_01);
			}
		}
	}

	if (IsLocallyControlled() && !PlayerMainUI)
	{
		if (UI_Editor_Main)
		{
			PlayerMainUI = CreateWidget<UUserWidget>(GetWorld(), UI_Editor_Main);
			if(PlayerMainUI)
			{
				//Editor_UI->AddToViewport();
				Editor_UI = Cast<UJSH_Editor_MainUI>(PlayerMainUI);
				Editor_UI->Begin_SavedPlayer(this);
				// Editor_UI->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}
	

	FName tag = TEXT("Rock");
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), tag, RockList);

	FName tag2 = TEXT("Station");
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), tag2, StationList);
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
	DOREPLIFETIME(AJSH_Player, CameraSpawn_b_On_Off);
	DOREPLIFETIME(AJSH_Player, Bool_MainLock);
	DOREPLIFETIME(AJSH_Player, Gizmo_Clicking_forError);
	DOREPLIFETIME(AJSH_Player, FirstGizmode);
	DOREPLIFETIME(AJSH_Player, Gizmo_TranslateMode);
	DOREPLIFETIME(AJSH_Player, Gizmo_ScaleMode);
	DOREPLIFETIME(AJSH_Player, Gizmo_RotateMode);
	DOREPLIFETIME(AJSH_Player, Editor_SpawnActor);
	DOREPLIFETIME(AJSH_Player, JPlayerController);
	DOREPLIFETIME(AJSH_Player, Lay_Distance);
	DOREPLIFETIME(AJSH_Player, Gizmo_Selected);
	DOREPLIFETIME(AJSH_Player, EnableEditSystem);
	DOREPLIFETIME(AJSH_Player, Gizmo_Detecting);
	DOREPLIFETIME(AJSH_Player, Bool_ZoomMode);
	
	DOREPLIFETIME(AJSH_Player, First_Clicked_SpawnActor);
	DOREPLIFETIME(AJSH_Player, Recent_Clicked_SpawnActor);
	DOREPLIFETIME(AJSH_Player, Now_Click_Actor);
	DOREPLIFETIME(AJSH_Player, EditorMode_On_B);
	
	DOREPLIFETIME(AJSH_Player, RockList);
	DOREPLIFETIME(AJSH_Player, Preset_Rock);
	

}

void AJSH_Player::Saved_PlayerController()
{
	JPlayerController = Cast<AJSH_PlayerController>(GetWorld()->GetFirstPlayerController());
	if (JPlayerController)
	{
		JPlayerController->bEnableTouchEvents = false;
	}
}


void AJSH_Player::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
			//UE_LOG(LogTemp, Error, TEXT("director add mapping"));
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

		EnhancedInputComponent->BindAction(IA_EditMode, ETriggerEvent::Started, this, &AJSH_Player::DisableEdit);
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


		// 4Key -> UI_Off
		EnhancedInputComponent->BindAction(IA_UI_OFF, ETriggerEvent::Started, this, &AJSH_Player::UI_Off);


		
		EnhancedInputComponent->BindAction(IA_Preset_On, ETriggerEvent::Started, this, &AJSH_Player::Preset_On_Off);
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
		// Record 함수를 끌고 오기 위한 GameInstance 
		ObsGamInstance = Cast<UJSH_OBSWebSocket>(GetGameInstance());
		if (CHJ_Instance == nullptr)
		{
			CHJ_Instance = Cast<UMainGameInstance>(GetGameInstance());
		}
		
		
		// 카메라 확대, 축소, 회전 , 초기화
		//CameraReset();

		if (JPlayerController == nullptr) Saved_PlayerController();
		// 인스턴스에 넣어둔 녹화 기능 시작
		//ObsGamInstance->StartRecord();

		if (JPlayerController)
		{
			JPlayerController->StartRecord();
		}
		
		if (!Record_b_On_Off)
		{
			if (!FlyMode_b_On_Off) FlyMode();
		
			FallGuys->SetCastShadow(false);


			if (RecordUI_01)
			{
				Origin_RecordUI->StartRecrod_Anim();
			}
			
			Record_b_On_Off = true;
		}
		else
		{
			if (RecordUI_01)
			{
				Origin_RecordUI->StopRecrod_Anim();
			}

			Record_b_On_Off = false;
		}
	}
}

void AJSH_Player::NetMulti_StartRecording_Implementation()
{

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
	// if (Bool_ZoomMode && !EditorMode_B) return;
	// (!EditorMode_B) return;
	// FlyMode가 아니라면 속도 조절 안 해 !!!
	//if (!GetCharacterMovement()->IsFlying()) return;
	
	float tempvalue = Value.Get<float>();

	MaxFlySpeed_C = MaxFlySpeed_C + tempvalue * 50;

	GetCharacterMovement()->MaxFlySpeed = MaxFlySpeed_C;

	//Origin_RecordUI->Speed_Update(FText::AsNumber(MaxFlySpeed_C));
	if (Origin_RecordUI)
	{
		Origin_RecordUI->Speed_Update(MaxFlySpeed_C);
	}

	if (HasAuthority())
	{
		NetMulti_FlySpeed(MaxFlySpeed_C);
	}
}


void AJSH_Player::NetMulti_FlySpeed_Implementation(float Value)
{
	MaxFlySpeed_C = Value;

	if (MaxFlySpeed_C <= 0.0f)
	{
		MaxFlySpeed_C = 0.0f;
	}
	
	GetCharacterMovement()->MaxFlySpeed = MaxFlySpeed_C;
}

void AJSH_Player::FlyMode()
{
	// 메인 플랫폼 일떄 기능 LOCK
	//if(!Bool_MainLock) return;

	GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	bUseControllerRotationPitch = true;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = true;

	if (HasAuthority())
	{
		NetMulti_FlyMode();
	}
}
void AJSH_Player::NetMulti_FlyMode_Implementation()
{
	GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	bUseControllerRotationPitch = true;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = true;
}


// (E) 위로 올라가는 - 레이 안 쏨
void AJSH_Player::Fly_Up_Down(const FInputActionValue& Value)
{
	if (GetCharacterMovement()->IsFlying())
	{
		if (!DisableEdit_b) return;

		// 입력 값에서 Up/Down 액션 값 추출
		Fly_Zvalue = Value.Get<float>();
		AddMovementInput(FVector(0.f, 0.f, 1.f), Fly_Zvalue);

		if (HasAuthority())
		{
			NetMulti_Fly_Up_Down(Fly_Zvalue);
		}
	}
}

void AJSH_Player::NetMulti_Fly_Up_Down_Implementation(const FInputActionValue& Value)
{
	AddMovementInput(FVector(0.f, 0.f, 1.f), Fly_Zvalue);
}


// (Q) 아래로 내려가는 -> 내려갈때 레이쏴서 일정 거리 가까워지면 FlyMode 종료 
void AJSH_Player::Fly_Down_Ray(const FInputActionValue& Value)
{
	if (GetCharacterMovement()->IsFlying())
	{
		// 입력 값에서 Up/Down 액션 값 추출
		Fly_Zvalue = Value.Get<float>();
		AddMovementInput(FVector(0.f, 0.f, 1.f), Fly_Zvalue);
		if (HasAuthority())
		{
			NetMulti_Fly_Down_Ray(Fly_Zvalue);
		}
	}
	
}



void AJSH_Player::NetMulti_Fly_Down_Ray_Implementation(const FInputActionValue& Value)
{
	AddMovementInput(FVector(0.f, 0.f, 1.f), Fly_Zvalue);
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

	// 카메라 관련 세팅 초기화
	CameraReset();

	
	Server_EditorMode();
}

void AJSH_Player::Server_EditorMode_Implementation()
{
	NetMulti_EditorMode();
}

void AJSH_Player::NetMulti_EditorMode_Implementation()
{
	// Editor Mode 꺼져 있다면
	if (!EditorMode_B)
	{
		// 에디터 모드 상태 체크 함수 (먼저 켜줘야 EnableEdit()가 돌아갈 수 있음)
		EditorMode_B = true;

		if (IsLocallyControlled())
		{
			if (Editor_UI)
			{
				Editor_UI->AddToViewport();
				Editor_UI->SelectMode2->SetVisibility(ESlateVisibility::Visible);
			}
			EnableEdit();  // EditorMode_B == false 이면 돌아가지 않음

			// // 기즈모 모드 임시
			// if (!FirstGizmode)
			// {
			// 	G_SelecteMode();
			// 	FirstGizmode = true;
			// }
		}
	}
	// Editor Mode 켜져 있다면
	else
	{
		//EditorMode_On_B = false;
		if (IsLocallyControlled())
		{

			// Gizmo 강제종료 문제
			if (Editor_SpawnActor != nullptr)
			{
				Editor_SpawnActor->OriginGizmo->Destroy();
			}
			
			DisableEdit();

			if (Editor_UI)
			{
				Editor_UI->RemoveFromParent();
			}
			// if (PlayerMainUI)
			// {
			// 	PlayerMainUI->RemoveFromParent();
			// 	PlayerMainUI = nullptr;
			// }
			// if (Editor_UI)
			// {
			// 	Editor_UI->SetVisibility(ESlateVisibility::Hidden);
			// }

			if (Origin_RecordUI)
			{
				Origin_RecordUI->SetVisibility(ESlateVisibility::Visible);
				//Origin_RecordUI->EditorMode_UI_Off();
			}

			// if (!RecordUI_01 && UI_Record_01)
			// {
			// 	RecordUI_01 = CreateWidget<UUserWidget>(GetWorld(), UI_Record_01);
			//
			// 	if(RecordUI_01)
			// 	{
			// 		RecordUI_01->AddToViewport();
			// 		Origin_RecordUI = Cast<UJSH_Record_UI>(RecordUI_01);
			// 	}
			// }

			UE_LOG(LogTemp, Warning, TEXT("`` delete"));
		}
		// Editor 모드 종료 시 저장된 EditorSpwanAcotr Name 삭제
		// JPlayerController->Editor_SpawnActor = nullptr;
		Editor_SpawnActor = nullptr; // 에디터 모드가 아닐떄 삭제 못하게
		
		EditorMode_B = false;
	}
}


void AJSH_Player::EnableEdit()
{
	// Editor 모드에서 마우스를 누른 상태로 , Editor 모드를 껏을때 , 꺼지고 나서 마우스를 떼면 EnableEdit가 On되는 문제가 있어 이렇게 막아줌
	// + 에디터 모드가 아닐때 함수가 실행되지 않도록
	if (!EditorMode_B) return;

	
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

	
	if (Editor_UI)
	{
		//Editor_UI->AddToViewport();
		Editor_UI->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}


	if (Origin_RecordUI)
	{
		//Origin_RecordUI->SetVisibility(ESlateVisibility::HitTestInvisible);
		Origin_RecordUI->SetVisibility(ESlateVisibility::Hidden);
		//Origin_RecordUI->EditorMode_UI_On();
	}

	// if (Editor_UI)
	// {
	// 	Editor_UI->SetVisibility(ESlateVisibility::Visible);
	// }
	Server_EnableEdit();
}

void AJSH_Player::Server_EnableEdit_Implementation()
{
	NetMulti_EnableEdit();
}

void AJSH_Player::NetMulti_EnableEdit_Implementation()
{
	DisableEdit_b = false;
	
	// 기즈모 Tick 제어를 위한 (Enable Edit 상태일 때 Ray 쏘도록)
	EnableEditSystem = true;
	
	// 마우스 우클릭을 안 하고 있으면 , ZoomMode를 Flase로
	Bool_ZoomMode = false;
}


void AJSH_Player::DisableEdit()
{
	if (!EditorMode_B) return;
	
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



	// if (PlayerMainUI)
	// {
	// 	PlayerMainUI->RemoveFromParent();
	// 	//PlayerMainUI = nullptr;
	//
	// 	 // if (Origin_RecordUI)
	// 	 // {
	// 	 // 	Origin_RecordUI->SetVisibility(ESlateVisibility::Hidden);
	// 	 // }
	// 	 //
	// 	 // if (!RecordUI_01->IsVisible())
	// 	 // {
	// 	 // 	RecordUI_01->SetVisibility(ESlateVisibility::Hidden);
	// 	 // }
	// }
	//
	// if (Editor_UI)
	// {
	// 	Editor_UI->SetVisibility(ESlateVisibility::Hidden);
	// }


	if (Editor_UI)
	{
		//Editor_UI->RemoveFromParent();
		Editor_UI->SetVisibility(ESlateVisibility::Hidden);
	}
	
	Server_DisableEdit();
}

void AJSH_Player::Server_DisableEdit_Implementation()
{
	NetMulti_DisableEdit();
}

void AJSH_Player::NetMulti_DisableEdit_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("``11"));
	DisableEdit_b = true;
	
	// 기즈모 Tick 제어를 위한 (Enable Edit 상태일 때 Ray 쏘도록)
	EnableEditSystem = false;
	
	// 마우스 우클릭 하고 있으면, ZoomMode를 True로
	if (!Bool_ZoomMode)
	{
		Bool_ZoomMode = true;
		UE_LOG(LogTemp, Error, TEXT("true"));
	}
	
	//EditorMode_B = false;
}






void AJSH_Player::CLickAndDel()
{
	// 클릭 하고 DelButton 눌러야 삭제 되도록
	ClickedEditorActor = !ClickedEditorActor;
}



// EditorActor를 클릭하면 그곳에서 자기 정보를 SaveEditorActor(AJSH_Editor_SpawnActor* ClickedActor) 여기로 전달 후 저장
void AJSH_Player::SaveEditorActor(AJSH_Editor_SpawnActor* ClickedActor)
{
	Server_SaveEditorActor(ClickedActor);
}

void AJSH_Player::Server_SaveEditorActor_Implementation(AJSH_Editor_SpawnActor* ClickedActor)
{
	NetMulti_SaveEditorActor_Implementation(ClickedActor);
}

void AJSH_Player::NetMulti_SaveEditorActor_Implementation(AJSH_Editor_SpawnActor* ClickedActor)
{
	Editor_SpawnActor = ClickedActor;
	

	// 이전 위치 돌아가는 함수 
	if (Recent_Clicked_SpawnActor == nullptr)
	{
		Recent_Clicked_SpawnActor = ClickedActor;
		//Recent_Clicked_SpawnActor->Onclicked();
	}
	else if (Recent_Clicked_SpawnActor != nullptr)
	{
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
void AJSH_Player::Save_Gizmo_SX(AActor* Gizmo_SX)
{
	Saved_Gizmo_SX = Cast<AJSH_Scale_GizmoX>(Gizmo_SX);
}
void AJSH_Player::Save_Gizmo_SY(AActor* Gizmo_SY)
{
	Saved_Gizmo_SY = Cast<AJSH_Scale_GizmoY>(Gizmo_SY);
}
void AJSH_Player::Save_Gizmo_SZ(AActor* Gizmo_SZ)
{
	Saved_Gizmo_SZ = Cast<AJSH_Scale_GizmoZ>(Gizmo_SZ);
}
void AJSH_Player::Save_Gizmo_SB(AActor* Gizmo_SB)
{
	Saved_Gizmo_SB = Cast<AJSH_Scale_GizmoBox>(Gizmo_SB);
}


// Rotate 정보 저장
void AJSH_Player::Save_Gizmo_RX(AActor* Gizmo_RX)
{
	Saved_Gizmo_RX = Cast<AJSH_Rotate_GizmoX>(Gizmo_RX);
}

void AJSH_Player::Save_Gizmo_RY(AActor* Gizmo_RY)
{
	Saved_Gizmo_RY = Cast<AJSH_Rotate_GizmoY>(Gizmo_RY);
}

void AJSH_Player::Save_Gizmo_RZ(AActor* Gizmo_RZ)
{
	Saved_Gizmo_RZ = Cast<AJSH_Rotate_GizmoZ>(Gizmo_RZ);
}


void AJSH_Player::EditorAcotorDestroy()
{
	NetMulti_EditorAcotorDestroy();
}
void AJSH_Player::NetMulti_EditorAcotorDestroy_Implementation()
{
	if (!EditorMode_B) return;
	if (DisableEdit_b) return;
	// 클릭 중에 q나 tap누르면 튕기는 오류 때문에 + 잡고 있을 떄 누르면 모드 바껴도 원래 상태로 
	if (Gizmo_Clicking_forError) return;
	
	if (Editor_SpawnActor != nullptr)
	{
		Editor_SpawnActor->DestroyThis();
	}
}


void AJSH_Player::Saved_Gizmo_Mode(float value)
{
	Saved_Gizmo_Mode_Value = value;
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
	if (Saved_Gizmo_SX == nullptr || Saved_Gizmo_SY == nullptr || Saved_Gizmo_SZ == nullptr || Saved_Gizmo_SB == nullptr) return;
	if (Saved_Gizmo_RX == nullptr || Saved_Gizmo_RY == nullptr || Saved_Gizmo_RZ == nullptr) return;

	UE_LOG(LogTemp, Warning, TEXT("g select"));
	Editor_SpawnActor = nullptr;
	
	Gizmo_TranslateMode = false;
	Gizmo_RotateMode = false;
	Gizmo_ScaleMode = false;

	// if (PlayerMainUI)
	// {
	// 	PlayerMainUI->select
	// }

	if (Editor_UI)
	{
		//Editor_UI->SelectMode->SetVisibility(ESlateVisibility::Hidden);
		Editor_UI->SelectMode2->SetVisibility(ESlateVisibility::Visible);

		//Editor_UI->TranslateMode->SetVisibility(ESlateVisibility::Visible);
		Editor_UI->TranslateMode2->SetVisibility(ESlateVisibility::Hidden);

		//Editor_UI->RotateMode->SetVisibility(ESlateVisibility::Visible);
		Editor_UI->RotateMode2->SetVisibility(ESlateVisibility::Hidden);

		//Editor_UI->ScaleMode->SetVisibility(ESlateVisibility::Visible);
		Editor_UI->ScaleMode2->SetVisibility(ESlateVisibility::Hidden);
	}

	Saved_Gizmo_TX->Visible_and_Collision_Off();
	Saved_Gizmo_TY->Visible_and_Collision_Off();
	Saved_Gizmo_TZ->Visible_and_Collision_Off();
	Saved_Gizmo_TB->Visible_and_Collision_Off();

	Saved_Gizmo_SX->Visible_and_Collision_Off();
	Saved_Gizmo_SY->Visible_and_Collision_Off();
	Saved_Gizmo_SZ->Visible_and_Collision_Off();
	Saved_Gizmo_SB->Visible_and_Collision_Off();

	
	Saved_Gizmo_RX->Visible_and_Collision_Off();
	Saved_Gizmo_RY->Visible_and_Collision_Off();
	Saved_Gizmo_RZ->Visible_and_Collision_Off();

	Saved_Gizmo_Mode(0);
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
	if (Saved_Gizmo_SX == nullptr || Saved_Gizmo_SY == nullptr || Saved_Gizmo_SZ == nullptr || Saved_Gizmo_SB == nullptr) return;
	if (Saved_Gizmo_RX == nullptr || Saved_Gizmo_RY == nullptr || Saved_Gizmo_RZ == nullptr) return;


	
	UE_LOG(LogTemp, Warning, TEXT("g translate"));
	Gizmo_TranslateMode = true;
	Gizmo_RotateMode = false;
	Gizmo_ScaleMode = false;


	if (Editor_UI)
	{
		//Editor_UI->SelectMode->SetVisibility(ESlateVisibility::Hidden);
		Editor_UI->SelectMode2->SetVisibility(ESlateVisibility::Hidden);

		//Editor_UI->TranslateMode->SetVisibility(ESlateVisibility::Visible);
		Editor_UI->TranslateMode2->SetVisibility(ESlateVisibility::Visible);

		//Editor_UI->RotateMode->SetVisibility(ESlateVisibility::Visible);
		Editor_UI->RotateMode2->SetVisibility(ESlateVisibility::Hidden);

		//Editor_UI->ScaleMode->SetVisibility(ESlateVisibility::Visible);
		Editor_UI->ScaleMode2->SetVisibility(ESlateVisibility::Hidden);
	}

	Saved_Gizmo_TX->Visible_and_Collision_On();
	Saved_Gizmo_TY->Visible_and_Collision_On();
	Saved_Gizmo_TZ->Visible_and_Collision_On();
	Saved_Gizmo_TB->Visible_and_Collision_On();

	Saved_Gizmo_SX->Visible_and_Collision_Off();
	Saved_Gizmo_SY->Visible_and_Collision_Off();
	Saved_Gizmo_SZ->Visible_and_Collision_Off();
	Saved_Gizmo_SB->Visible_and_Collision_Off();

	Saved_Gizmo_RX->Visible_and_Collision_Off();
	Saved_Gizmo_RY->Visible_and_Collision_Off();
	Saved_Gizmo_RZ->Visible_and_Collision_Off();

	Saved_Gizmo_Mode(1);
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
	if (Saved_Gizmo_SX == nullptr || Saved_Gizmo_SY == nullptr || Saved_Gizmo_SZ == nullptr || Saved_Gizmo_SB == nullptr) return;
	if (Saved_Gizmo_RX == nullptr || Saved_Gizmo_RY == nullptr || Saved_Gizmo_RZ == nullptr) return;

	UE_LOG(LogTemp, Warning, TEXT("g  rotate"));
	Gizmo_TranslateMode = false;
	Gizmo_RotateMode = true;
	Gizmo_ScaleMode = false;



	if (Editor_UI)
	{
		//Editor_UI->SelectMode->SetVisibility(ESlateVisibility::Hidden);
		Editor_UI->SelectMode2->SetVisibility(ESlateVisibility::Hidden);

		//Editor_UI->TranslateMode->SetVisibility(ESlateVisibility::Visible);
		Editor_UI->TranslateMode2->SetVisibility(ESlateVisibility::Hidden);

		//Editor_UI->RotateMode->SetVisibility(ESlateVisibility::Visible);
		Editor_UI->RotateMode2->SetVisibility(ESlateVisibility::Visible);

		//Editor_UI->ScaleMode->SetVisibility(ESlateVisibility::Visible);
		Editor_UI->ScaleMode2->SetVisibility(ESlateVisibility::Hidden);
	}

	Saved_Gizmo_TX->Visible_and_Collision_Off();
	Saved_Gizmo_TY->Visible_and_Collision_Off();
	Saved_Gizmo_TZ->Visible_and_Collision_Off();
	Saved_Gizmo_TB->Visible_and_Collision_Off();

	Saved_Gizmo_SX->Visible_and_Collision_Off();
	Saved_Gizmo_SY->Visible_and_Collision_Off();
	Saved_Gizmo_SZ->Visible_and_Collision_Off();
	Saved_Gizmo_SB->Visible_and_Collision_Off();

	Saved_Gizmo_RX->Visible_and_Collision_On();
	Saved_Gizmo_RY->Visible_and_Collision_On();
	Saved_Gizmo_RZ->Visible_and_Collision_On();

	Saved_Gizmo_Mode(2);
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
	if (Saved_Gizmo_SX == nullptr || Saved_Gizmo_SY == nullptr || Saved_Gizmo_SZ == nullptr || Saved_Gizmo_SB == nullptr) return;
	if (Saved_Gizmo_RX == nullptr || Saved_Gizmo_RY == nullptr || Saved_Gizmo_RZ == nullptr) return;


	UE_LOG(LogTemp, Warning, TEXT("g scale"));
	Gizmo_TranslateMode = false;
	Gizmo_RotateMode = false;
	Gizmo_ScaleMode = true;


	if (Editor_UI)
	{
		//Editor_UI->SelectMode->SetVisibility(ESlateVisibility::Hidden);
		Editor_UI->SelectMode2->SetVisibility(ESlateVisibility::Hidden);

		//Editor_UI->TranslateMode->SetVisibility(ESlateVisibility::Visible);
		Editor_UI->TranslateMode2->SetVisibility(ESlateVisibility::Hidden);

		//Editor_UI->RotateMode->SetVisibility(ESlateVisibility::Visible);
		Editor_UI->RotateMode2->SetVisibility(ESlateVisibility::Hidden);

		//Editor_UI->ScaleMode->SetVisibility(ESlateVisibility::Visible);
		Editor_UI->ScaleMode2->SetVisibility(ESlateVisibility::Visible);
	}



	Saved_Gizmo_TX->Visible_and_Collision_Off();
	Saved_Gizmo_TY->Visible_and_Collision_Off();
	Saved_Gizmo_TZ->Visible_and_Collision_Off();
	Saved_Gizmo_TB->Visible_and_Collision_Off();

	Saved_Gizmo_SX->Visible_and_Collision_On();
	Saved_Gizmo_SY->Visible_and_Collision_On();
	Saved_Gizmo_SZ->Visible_and_Collision_On();
	Saved_Gizmo_SB->Visible_and_Collision_On();


	Saved_Gizmo_RX->Visible_and_Collision_Off();
	Saved_Gizmo_RY->Visible_and_Collision_Off();
	Saved_Gizmo_RZ->Visible_and_Collision_Off();

	Saved_Gizmo_Mode(3);
}


void AJSH_Player::Gizmo_Detect()
{
	if (!EditorMode_B) return;
	if (DisableEdit_b) return;
	if (Saved_Gizmo_TX == nullptr || Saved_Gizmo_TY == nullptr || Saved_Gizmo_TZ == nullptr || Saved_Gizmo_TB == nullptr) return;
	if (Saved_Gizmo_SX == nullptr || Saved_Gizmo_SY == nullptr || Saved_Gizmo_SZ == nullptr || Saved_Gizmo_SB == nullptr) return;
	if (Saved_Gizmo_RX == nullptr || Saved_Gizmo_RY == nullptr || Saved_Gizmo_RZ == nullptr) return;



	
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

		/// Translate
		if (Saved_Gizmo_TX != nullptr)
		{
			if (HitResult.GetActor()  == Saved_Gizmo_TX)
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

		/// Sclae
		if (Saved_Gizmo_SX != nullptr)
		{
			if (HitResult.GetActor()  == Saved_Gizmo_SX)
			{
				Saved_Gizmo_SX->BeginCursorOver();

				Saved_Gizmo_SY->EndCursorOver();
				Saved_Gizmo_SZ->EndCursorOver();
				Saved_Gizmo_SB->EndCursorOver();
			}
		}

		if (Saved_Gizmo_SY != nullptr)
		{
			if (HitResult.GetActor()  == Saved_Gizmo_SY)
			{
				Saved_Gizmo_SY->BeginCursorOver();

				Saved_Gizmo_SX->EndCursorOver();
				Saved_Gizmo_SZ->EndCursorOver();
				Saved_Gizmo_SB->EndCursorOver();
			}
		}


		if (Saved_Gizmo_SZ != nullptr)
		{
			if (HitResult.GetActor()  == Saved_Gizmo_SZ)
			{
				Saved_Gizmo_SZ->BeginCursorOver();

				Saved_Gizmo_SX->EndCursorOver();
				Saved_Gizmo_SY->EndCursorOver();
				Saved_Gizmo_SB->EndCursorOver();
			}
		}

		if (Saved_Gizmo_SB != nullptr)
		{
			if (HitResult.GetActor()  == Saved_Gizmo_SB)
			{
				Saved_Gizmo_SB->BeginCursorOver();

				Saved_Gizmo_SX->EndCursorOver();
				Saved_Gizmo_SY->EndCursorOver();
				Saved_Gizmo_SZ->EndCursorOver();
			}
		}


		// Rotate

		if (Saved_Gizmo_RX != nullptr)
		{
			if (HitResult.GetActor()  == Saved_Gizmo_RX)
			{
				Saved_Gizmo_RX->BeginCursorOver();

				Saved_Gizmo_RY->EndCursorOver();
				Saved_Gizmo_RZ->EndCursorOver();
			}
		}

		if (Saved_Gizmo_RY != nullptr)
		{
			if (HitResult.GetActor()  == Saved_Gizmo_RY)
			{
				Saved_Gizmo_RY->BeginCursorOver();

				Saved_Gizmo_RX->EndCursorOver();
				Saved_Gizmo_RZ->EndCursorOver();
			}
		}

		if (Saved_Gizmo_RZ != nullptr)
		{
			if (HitResult.GetActor()  == Saved_Gizmo_RZ)
			{
				Saved_Gizmo_RZ->BeginCursorOver();

				Saved_Gizmo_RX->EndCursorOver();
				Saved_Gizmo_RY->EndCursorOver();
			}
		}

	}
	else
	{
		if (Gizmo_Detecting == true) Gizmo_Detecting = false;
		
		HitResult.Reset();

		// Translate
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

		// Sclae
		if (Saved_Gizmo_SX != nullptr)
		{
			Saved_Gizmo_SX->EndCursorOver();
		}
		if (Saved_Gizmo_SY != nullptr)
		{
			Saved_Gizmo_SY->EndCursorOver();
		}
		if (Saved_Gizmo_SZ != nullptr)
		{
			Saved_Gizmo_SZ->EndCursorOver();
		}
		if (Saved_Gizmo_SB != nullptr)
		{
			Saved_Gizmo_SB->EndCursorOver();
		}


		// Rotate
		if (Saved_Gizmo_RX != nullptr)
		{
			Saved_Gizmo_RX->EndCursorOver();
		}

		if (Saved_Gizmo_RY != nullptr)
		{
			Saved_Gizmo_RY->EndCursorOver();
		}

		if (Saved_Gizmo_RZ != nullptr)
		{
			Saved_Gizmo_RZ->EndCursorOver();
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


	// Translate
	if (Saved_Gizmo_TX != nullptr)
	{
		if (HitResult.GetActor() == Saved_Gizmo_TX)
		{
			Saved_Gizmo_TX->GOnClicked();
			Clicked_X = true;
		}
	}
	
	if (Saved_Gizmo_TY != nullptr)
	{
		if (HitResult.GetActor() == Saved_Gizmo_TY)
		{
			Saved_Gizmo_TY->GOnClicked();
			Clicked_Y = true;
		}
	}

	if (Saved_Gizmo_TZ != nullptr)
	{
		if (HitResult.GetActor() == Saved_Gizmo_TZ)
		{
			Saved_Gizmo_TZ->GOnClicked();
			Clicked_Z = true;
		}
	}
	
	if (Saved_Gizmo_TB != nullptr)
	{
		if (HitResult.GetActor() == Saved_Gizmo_TB)
		{
			Saved_Gizmo_TB->GOnClicked();
			Clicked_B = true;
		}
	}

	// Scale
	if (Saved_Gizmo_SX != nullptr)
	{
		if (HitResult.GetActor() == Saved_Gizmo_SX)
		{
			Saved_Gizmo_SX->GOnClicked();
			Clicked_X = true;
		}
	}

	if (Saved_Gizmo_SY != nullptr)
	{
		if (HitResult.GetActor() == Saved_Gizmo_SY)
		{
			Saved_Gizmo_SY->GOnClicked();
			Clicked_Y = true;
		}
	}

	if (Saved_Gizmo_SZ != nullptr)
	{
		if (HitResult.GetActor() == Saved_Gizmo_SZ)
		{
			Saved_Gizmo_SZ->GOnClicked();
			Clicked_Z = true;
		}
	}
	
	if (Saved_Gizmo_SB != nullptr)
	{
		if (HitResult.GetActor() == Saved_Gizmo_SB)
		{
			Saved_Gizmo_SB->GOnClicked();
			Clicked_B = true;
		}
	}
	
	// Rotate
	if (Saved_Gizmo_RX != nullptr)
	{
		if (HitResult.GetActor() == Saved_Gizmo_RX)
		{
			Saved_Gizmo_RX->GOnClicked();
			Clicked_B = true;
		}
	}

	if (Saved_Gizmo_RY != nullptr)
	{
		if (HitResult.GetActor() == Saved_Gizmo_RY)
		{
			Saved_Gizmo_RY->GOnClicked();
			Clicked_B = true;
		}
	}

	if (Saved_Gizmo_RZ != nullptr)
	{
		if (HitResult.GetActor() == Saved_Gizmo_RZ)
		{
			Saved_Gizmo_RZ->GOnClicked();
			Clicked_B = true;
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

	Saved_Gizmo_TX->HandleMouseReleaseOutsideActor();
	Saved_Gizmo_TY->HandleMouseReleaseOutsideActor();
	Saved_Gizmo_TZ->HandleMouseReleaseOutsideActor();
	Saved_Gizmo_TB->HandleMouseReleaseOutsideActor();

	Saved_Gizmo_SX->HandleMouseReleaseOutsideActor();
	Saved_Gizmo_SY->HandleMouseReleaseOutsideActor();
	Saved_Gizmo_SZ->HandleMouseReleaseOutsideActor();
	Saved_Gizmo_SB->HandleMouseReleaseOutsideActor();


	Saved_Gizmo_RX->HandleMouseReleaseOutsideActor();
	Saved_Gizmo_RY->HandleMouseReleaseOutsideActor();
	Saved_Gizmo_RZ->HandleMouseReleaseOutsideActor();

	// if (Clicked_X)
	// {
	// 	if (Gizmo_TranslateMode) Saved_Gizmo_TX->HandleMouseReleaseOutsideActor();
	// 	else if (Gizmo_ScaleMode) Saved_Gizmo_SX->HandleMouseReleaseOutsideActor();
	// 	else if (Gizmo_RotateMode) Saved_Gizmo_RX->HandleMouseReleaseOutsideActor();
	// }
	// if (Clicked_Y)
	// {
	// 	if (Gizmo_TranslateMode) Saved_Gizmo_TY->HandleMouseReleaseOutsideActor();
	// 	else if (Gizmo_ScaleMode) Saved_Gizmo_SY->HandleMouseReleaseOutsideActor();
	// 	else if (Gizmo_RotateMode) Saved_Gizmo_RY->HandleMouseReleaseOutsideActor();
	// 	//Saved_Gizmo_TY->HandleMouseReleaseOutsideActor();
	// }
	// if (Clicked_Z)
	// {
	// 	if (Gizmo_TranslateMode) Saved_Gizmo_TZ->HandleMouseReleaseOutsideActor();
	// 	else if (Gizmo_ScaleMode) Saved_Gizmo_SZ->HandleMouseReleaseOutsideActor();
	// 	else if (Gizmo_RotateMode) Saved_Gizmo_RZ->HandleMouseReleaseOutsideActor();
	//
	// 	//Saved_Gizmo_TZ->HandleMouseReleaseOutsideActor();
	// }
	// if (Clicked_B)
	// {
	// 	if (Gizmo_TranslateMode) Saved_Gizmo_TB->HandleMouseReleaseOutsideActor();
	// 	else if (Gizmo_ScaleMode) Saved_Gizmo_SB->HandleMouseReleaseOutsideActor();
	//
	// 	//Saved_Gizmo_TB->HandleMouseReleaseOutsideActor();
	// }

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
			RecordCamera->SetFieldOfView(RecordCamera->FieldOfView);
		}

		if (Origin_RecordUI)
		{
			Origin_RecordUI->Zoom_Update(RecordCamera->FieldOfView);
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

		if (Origin_RecordUI)
		{
			Origin_RecordUI->Zoom_Update(RecordCamera->FieldOfView);
		}
	}
}

void AJSH_Player::Camera_Zoom_Default()
{
	if (EditorMode_B) return;
	
		
	RecordCamera->SetFieldOfView(90.0f);

	if (Origin_RecordUI)
	{
		//Origin_RecordUI->Zoom_Update(RecordCamera->FieldOfView);
		Origin_RecordUI->Reset_Zoom_Update();
	}
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


	if (Origin_RecordUI)
	{
		Origin_RecordUI->CameraUI_Rotate_Update(CurrentAngl);
	}
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


	if (Origin_RecordUI)
	{
		Origin_RecordUI->CameraUI_Rotate_Update(CurrentAngl);
	}
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
	if (Origin_RecordUI)
	{
		Origin_RecordUI->CameraUI_Rotate_Update(CurrentAngl);
	}
}

void AJSH_Player::CameraReset()
{
	// 카메라 원상 복구 해주는 코드
	RecordCamera->SetFieldOfView(90.0f);
	RecordCamera->SetRelativeRotation(DefaultCameraleaning);
	CurrentAngl = 0;
	UE_LOG(LogTemp, Error, TEXT("CameraReset"));

	// if (Origin_RecordUI)
	// {
	// 	Origin_RecordUI->CameraUI_Rotate_Update(CurrentAngl);
	// 	//Origin_RecordUI->Sensitivity_Update(MouseSensitivityYaw);
	// 	Origin_RecordUI->CameraUI_Rotate_Update(CurrentAngl);
	// 	Origin_RecordUI->Zoom_Update(RecordCamera->FieldOfView);
	// 	Origin_RecordUI->Reset_Zoom_Update();
	// }
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
	if (Origin_RecordUI)
	{
		Origin_RecordUI->Sensitivity_Update(MouseSensitivityYaw);
	}
	
	UE_LOG(LogTemp, Error, TEXT("YYY %f"), MouseSensitivityYaw);
	UE_LOG(LogTemp, Error, TEXT("PPP %f"), MouseSensitivityPitch);
}



#pragma endregion


#pragma region Session

void AJSH_Player::Esc()
{
	// Editor 모드일때 ESC 누르면 선택 없어지는거
	if (EditorMode_B == false)
	{
		if (CHJ_Instance == nullptr) CHJ_Instance = Cast<UMainGameInstance>(GetGameInstance());

		// 종료하겠습니까 위젯 하나 뛰어줘야할듯
		if (HasAuthority())
		{
			CHJ_Instance->ExitSession();
			UE_LOG(LogTemp, Error, TEXT("ESC"));
		}
		// if (!EditorMode_B) return;
		// if (DisableEdit_b) return;
		// // 클릭 중에 q나 tap누르면 튕기는 오류 때문에 + 잡고 있을 떄 누르면 모드 바껴도 원래 상태로 
		// if (Gizmo_Clicking_forError) return;
		// Editor_SpawnActor = nullptr;
	}
}

void AJSH_Player::UI_Off()
{
	if (HasAuthority())
	{
		if (RecordUI_01)
		{
			RecordUI_01->RemoveFromParent();
			RecordUI_01 = nullptr;  
		}
	}
}



#pragma endregion



void AJSH_Player::Preset_On_Off()
{

	if (RockList.Num() > 0)
	{
		AActor* firstRock = RockList[0];
		if (firstRock)
		{
			UE_LOG(LogTemp, Warning, TEXT("p111111"));
			Server_Preset_On_Off(firstRock);
		}
	}

	//
	// for (AActor* Pop : RockList)
	// {
	// 	if (Pop)
	// 	{
	// 		UE_LOG(LogTemp, Warning, TEXT("p111111"));
	//
	// 		Server_Preset_On_Off(Pop);
	// 		break;
	// 	}
	// }
}


void AJSH_Player::Server_Preset_On_Off_Implementation(AActor* rock)
{
	NetMulti_Preset_On_Off(rock);
}


void AJSH_Player::NetMulti_Preset_On_Off_Implementation(AActor* rock)
{
	Preset_Rock = Cast<AJSH_Preset>(rock);
	if (Preset_Rock) Preset_Rock->Hidden_On_Off2();

}




void AJSH_Player::Preset_On_Off_Station()
{
	FName tag2 = TEXT("Station");
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), tag2, StationList);
	
	for (AActor* sta : StationList)
	{
		if (sta)
		{
			UE_LOG(LogTemp, Warning, TEXT("p111111"));

			Server_Preset_On_Off(sta);
			break;
		}
	}
}

void AJSH_Player::Server_Preset_On_Off_Station_Implementation(AActor* Station)
{
	NetMulti_Preset_On_Off_Station(Station);
}

void AJSH_Player::NetMulti_Preset_On_Off_Station_Implementation(AActor* Station)
{
	Preset_Station = Cast<AJSH_Preset>(Station);

	if (Preset_Station) Preset_Station->Hidden_On_Off();
}








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