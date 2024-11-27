// Fill out your copyright notice in the Description page of Project Settings.


#include "../JSH/JSH_BetaPlayer.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "JSH_PlayerController.h"
#include "BetaPl/JSH_TheaterSpawnActor.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"
#include "MetaStudios/CHJ/MainGameInstance.h"
#include "Net/UnrealNetwork.h"
#include "Widget/JSH_VideoStart.h"

AJSH_BetaPlayer::AJSH_BetaPlayer()
{

	SetReplicates(true);


	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	ConstructorHelpers::FObjectFinder<USkeletalMesh> TMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/JSH/Asset/Corgi/untitled.untitled'"));
	if (TMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(TMesh.Object);
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -30), FRotator(0, -90, 0));
		GetMesh()->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
	}
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;


	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate


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


	HandComp = CreateDefaultSubobject<USceneComponent>(TEXT("HandComp"));
	HandComp->SetupAttachment(GetMesh() , TEXT("LeftHand_Pop"));


	// // 팝콘 스폰을 위한
	static ConstructorHelpers::FClassFinder<AActor> PopBPClass(TEXT("/Game/JSH/BP/BP_PopCorn.BP_PopCorn_C"));
	if (PopBPClass.Succeeded())
	{
		PopClass = PopBPClass.Class;
	}
}

void AJSH_BetaPlayer::BeginPlay()
{
	Super::BeginPlay();

	// // 태어날 때 모든 AX 목록 기억
	// FName tag = TEXT("Pop");
	// UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld() , AActor::StaticClass() , tag , PopList);


	JPlayerController = Cast<AJSH_PlayerController>(GetWorld()->GetFirstPlayerController());
	if (JPlayerController)
	{
		UE_LOG(LogTemp, Warning, TEXT("bb1"));
		JPlayerController->bEnableTouchEvents = false;
	}

	if (IsLocallyControlled())
	{
		if (UI_StartVideo)
		{
			StartVideoUI = CreateWidget<UUserWidget>(GetWorld(), UI_StartVideo);
			UE_LOG(LogTemp, Warning, TEXT("bb2"));
		
			if(StartVideoUI)
			{
				StartVideoUI->SetIsEnabled(true);
				StartVideoUI->AddToViewport();
			
				UE_LOG(LogTemp, Warning, TEXT("bb3"));
			
				UE_LOG(LogTemp, Warning, TEXT("bb4"));
				Origin_StartVideoUI = Cast<UJSH_VideoStart>(StartVideoUI);
				Origin_StartVideoUI->Saved_BetaPlayer(this);
				UE_LOG(LogTemp, Warning, TEXT("bb5"));
				MouseCursor_On();
			}
		}
	}

}



void AJSH_BetaPlayer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AJSH_BetaPlayer, PopModeON);
	DOREPLIFETIME(AJSH_BetaPlayer, PopAnimationplay);
	DOREPLIFETIME(AJSH_BetaPlayer, GrabDistance);
	DOREPLIFETIME(AJSH_BetaPlayer, PopMontage);
	DOREPLIFETIME(AJSH_BetaPlayer, GrabPopActor);
	DOREPLIFETIME(AJSH_BetaPlayer, PopOn);
	DOREPLIFETIME(AJSH_BetaPlayer, BHasPop);
	DOREPLIFETIME(AJSH_BetaPlayer, PopList);
	DOREPLIFETIME(AJSH_BetaPlayer, HandComp);
	DOREPLIFETIME(AJSH_BetaPlayer, CapsuleComp);
	DOREPLIFETIME(AJSH_BetaPlayer, PopGrab_O);
	DOREPLIFETIME(AJSH_BetaPlayer, OriginPop);
	//DOREPLIFETIME(AJSH_BetaPlayer, PopClass);
	//DOREPLIFETIME(AJSH_BetaPlayer, PopAct);
	//DOREPLIFETIME(AJSH_BetaPlayer, SpawnPop);
	DOREPLIFETIME(AJSH_BetaPlayer, CapsuleComp);
	DOREPLIFETIME(AJSH_BetaPlayer, CapsuleComp);
}

void AJSH_BetaPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FName tag = TEXT("Pop");
	UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld() , AActor::StaticClass() , tag , PopList);
}



void AJSH_BetaPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
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


		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AJSH_BetaPlayer::Move);


		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AJSH_BetaPlayer::Look);


		EnhancedInputComponent->BindAction(IA_Grab, ETriggerEvent::Started, this, &AJSH_BetaPlayer::Grab);
		
		EnhancedInputComponent->BindAction(IA_SpawnPop, ETriggerEvent::Started, this, &AJSH_BetaPlayer::Spawn_Pop);
	}
	else
	{
		//UE_LOG(
	}
}

void AJSH_BetaPlayer::Move(const FInputActionValue& Value)
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

void AJSH_BetaPlayer::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}



void AJSH_BetaPlayer::Grab()
{
	if ( BHasPop )
	{
		if (HasAuthority()) UE_LOG(LogTemp, Warning, TEXT("g4"));
		MyReleasePop();
		
		Server_Grab(BHasPop);
	}
	else
	{
		Server_Grab(BHasPop);
		MyTakePop();
	}
	
	
}

void AJSH_BetaPlayer::Server_Grab_Implementation(bool gtrue)
{
	NetMulti_Grab(gtrue);
}


void AJSH_BetaPlayer::NetMulti_Grab_Implementation(bool gtrue)
{
	if (gtrue)
	{
		PopModeON = false;
	}
	else
	{
		FName tag = TEXT("Pop");
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), tag, PopList);
	}
}



////////////////////////
void AJSH_BetaPlayer::MyTakePop()
{
	Server_MyTakePop();
}

void AJSH_BetaPlayer::Server_MyTakePop_Implementation()
{
	NetMulti_MyTakePop();
}

void AJSH_BetaPlayer::NetMulti_MyTakePop_Implementation()
{
	FName tag = TEXT("Pop");
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), tag, PopList);
	
	for ( AActor* Pop : PopList )
	{
		float tempDist = GetDistanceTo(Pop);

		if ( tempDist > GrabDistance ) continue;

		//서버 쪽에서 여기서 자꾸 막힘... 왜 그런지 몰르겠음 ....
		if (Pop->GetOwner() != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("000000"));
			continue;
		}
		OriginPop = Cast<AJSH_TheaterSpawnActor>(Pop);
		//OriginPop->SET_JOwner(this);
		//OriginPop->SetOwner(this);
		GrabPopActor = Pop;
		GrabPopActor->SetOwner(this);
		AttachPop(GrabPopActor);
		BHasPop = true;
		PopModeON = true;
		PopGrab_O = true;
		
		break;
	}
	
	
}



////////////////////////
void AJSH_BetaPlayer::MyReleasePop()
{
	Server_MyReleasePop();
}


void AJSH_BetaPlayer::Server_MyReleasePop_Implementation()
{
	NetMulti_MyReleasePop();
}


void AJSH_BetaPlayer::NetMulti_MyReleasePop_Implementation()
{
	if ( false == BHasPop) return;
	
	if ( BHasPop )
	{
		BHasPop = false;
	}


	if ( GrabPopActor )
	{
		DetachPop(GrabPopActor);

		GrabPopActor->SetOwner(nullptr);

		GrabPopActor = nullptr;
	}
	
	PopModeON = false;
	PopGrab_O = false;
}



////////////////////////
void AJSH_BetaPlayer::AttachPop(AActor* PopActor)
{
	Server_AttachPop(PopActor);
}
void AJSH_BetaPlayer::Server_AttachPop_Implementation(AActor* PopActor)
{
	NetMulti_AttachPop(PopActor);
}

void AJSH_BetaPlayer::NetMulti_AttachPop_Implementation(AActor* PopActor)
{
	CapsuleComp = OriginPop->GetComponentByClass<UCapsuleComponent>();
	check(CapsuleComp);
	CapsuleComp->SetSimulatePhysics(false);
	CapsuleComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	CapsuleComp->AttachToComponent(HandComp , FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	
}


////////////////////////
void AJSH_BetaPlayer::DetachPop(AActor* PopActor)
{
	Server_DetachPop(CapsuleComp);

}
void AJSH_BetaPlayer::Server_DetachPop_Implementation(UCapsuleComponent* cap)
{
	NetMulti_DetachPop(CapsuleComp);
}

void AJSH_BetaPlayer::NetMulti_DetachPop_Implementation(UCapsuleComponent* cap)
{
	CapsuleComp = OriginPop->GetComponentByClass<UCapsuleComponent>();
	check(CapsuleComp);
	CapsuleComp->SetSimulatePhysics(true);
	CapsuleComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	CapsuleComp->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);

	
	// cap->SetSimulatePhysics(true);
	// cap->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	// cap->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
}

//////
void AJSH_BetaPlayer::Spawn_Pop()
{
	FVector ThisForwardVector = this->GetActorForwardVector();
	FVector ThisLocation = this->GetActorLocation() + ThisForwardVector * 10;
	ThisLocation.Z += 100.0f; 
	
	FRotator Thisrotator = this->GetActorRotation();
	UE_LOG(LogTemp, Warning, TEXT("p111111111111111111111"));
	
	Server_Spawn_Pop(ThisLocation, Thisrotator);
}

void AJSH_BetaPlayer::Server_Spawn_Pop_Implementation(FVector slo, FRotator sro)
{
	UE_LOG(LogTemp, Warning, TEXT("serverRPC"));
	if (PopClass)
	{
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		
		PopAct = GetWorld()->SpawnActor<AJSH_TheaterSpawnActor>(PopClass, slo, sro, SpawnParams);
		UE_LOG(LogTemp, Warning, TEXT("%s %s"), GetWorld()->GetNetMode()==ENetMode::NM_Client ? TEXT("Client") : GetWorld()->GetNetMode()==NM_Standalone ? TEXT("Standalone") : TEXT("Server"), *FString("Spawn"));

		// 귀엽게 위로 살짝 튀도록
		if (PopAct)
		{
			UPrimitiveComponent* PopRootComponent = Cast<UPrimitiveComponent>(PopAct->GetRootComponent());
			if (PopRootComponent)
			{
				FVector Impulse(0.0f, 0.0f, 500.0f); // 위쪽으로 향하는 임펄스 벡터
				PopRootComponent->AddImpulse(Impulse, NAME_None, true); // Root Component에 임펄스를 추가합니다.
			}
		}

	}
	//NetMulit_Pop(slo, sro);
}

void AJSH_BetaPlayer::NetMulit_Pop_Implementation(FVector slo, FRotator sro)
{
	
	// if (HasAuthority()) UE_LOG(LogTemp, Warning, TEXT("hhhc"));
	// if (IsLocallyControlled()) UE_LOG(LogTemp, Warning, TEXT("lllc"));
	// if (PopClass)
	// {
	// 	//UE_LOG(LogTemp, Warning, TEXT("p333333333333333333333333333333333"));
	// 	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	// 	
	// 	PopAct = GetWorld()->SpawnActor<AJSH_TheaterSpawnActor>(PopClass, slo, sro, SpawnParams);
	// 	UE_LOG(LogTemp, Warning, TEXT("%s %s"), GetWorld()->GetNetMode()==ENetMode::NM_Client ? TEXT("Client") : GetWorld()->GetNetMode()==NM_Standalone ? TEXT("Standalone") : TEXT("Server"), *FString("Spawn"));
	//
	// 	//UE_LOG(LogTemp, Warning, TEXT("p44444444444444444444444444444"));
	// 	//SpawnPop = Cast<AJSH_TheaterSpawnActor>(PopAct);
	// 	// 귀엽게 위로 살짝 튀도록
	// 	if (PopAct)
	// 	{
	// 		UPrimitiveComponent* PopRootComponent = Cast<UPrimitiveComponent>(PopAct->GetRootComponent());
	// 		if (PopRootComponent)
	// 		{
	// 			FVector Impulse(0.0f, 0.0f, 500.0f); // 위쪽으로 향하는 임펄스 벡터
	// 			PopRootComponent->AddImpulse(Impulse, NAME_None, true); // Root Component에 임펄스를 추가합니다.
	// 		}
	// 	}
	//
	// }
}


void AJSH_BetaPlayer::Esc()
{
	// 종료하겠습니까 위젯 하나 뛰어줘야할듯
	if (CHJ_Instance == nullptr) CHJ_Instance = Cast<UMainGameInstance>(GetGameInstance());
	CHJ_Instance->ExitSession();
}

void AJSH_BetaPlayer::MouseCursor_On()
{
	
	UE_LOG(LogTemp, Warning, TEXT("bb6"));
	if (!JPlayerController)
	{
		UE_LOG(LogTemp, Warning, TEXT("bb7"));
		JPlayerController = Cast<AJSH_PlayerController>(GetWorld()->GetFirstPlayerController());
		if (JPlayerController)
		{
			JPlayerController->bEnableTouchEvents = false;
		
			UE_LOG(LogTemp, Error, TEXT("Begin_Jcontorller1111"));
			// 플레이어 컨트롤러에 Director 저장
			JPlayerController->SaveOriginCharacter();
			UE_LOG(LogTemp, Warning, TEXT("bb8"));
		}
	}

	if (IsLocallyControlled())
	{
		if (JPlayerController)
		{
			GetMovementComponent()->SetComponentTickEnabled(false);
			
			UE_LOG(LogTemp, Warning, TEXT("bb9"));
			JPlayerController->SetIgnoreLookInput(true);
		
			// 마우스 커서 보이게 설정
			JPlayerController->bShowMouseCursor = true;
			JPlayerController->bEnableClickEvents = true;
			JPlayerController->bEnableMouseOverEvents = true;
		
			FInputModeGameAndUI InputMode;
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
			InputMode.SetHideCursorDuringCapture(false);
			JPlayerController->SetInputMode(InputMode);
			UE_LOG(LogTemp, Warning, TEXT("bb10"));
		}
	}


}

void AJSH_BetaPlayer::MouseCursor_Off()
{
	if (IsLocallyControlled())
	{
		GetMovementComponent()->SetComponentTickEnabled(true);
		if (JPlayerController)
		{
			JPlayerController->SetIgnoreLookInput(false);
		
			JPlayerController->bShowMouseCursor = false;
			JPlayerController->bEnableClickEvents = false;
			JPlayerController->bEnableMouseOverEvents = false;
			JPlayerController->SetInputMode(FInputModeGameOnly());
		}

		// if (Origin_StartVideoUI)
		// {
		// 	Origin_StartVideoUI->RemoveFromParent();
		// }
	}

}

void AJSH_BetaPlayer::Overlap_video()
{
	if (IsLocallyControlled())
	{
		Origin_StartVideoUI->OverlapImage->SetVisibility(ESlateVisibility::Visible);
	}
}

void AJSH_BetaPlayer::Overlap_End_video()
{
	if (IsLocallyControlled())
	{
		Origin_StartVideoUI->OverlapImage->SetVisibility(ESlateVisibility::Hidden);
	}
}
