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
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

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
	HandComp->SetupAttachment(GetMesh() , TEXT("RightHand"));
}

void AJSH_BetaPlayer::BeginPlay()
{
	Super::BeginPlay();

	// 태어날 때 모든 AX 목록 기억
	FName tag = TEXT("Pop");
	UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld() , AActor::StaticClass() , tag , PopList);
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



}

void AJSH_BetaPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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
	if (HasAuthority()) UE_LOG(LogTemp, Warning, TEXT("g1"));

	Server_Grab();
}

void AJSH_BetaPlayer::Server_Grab_Implementation()
{
	if (HasAuthority()) UE_LOG(LogTemp, Warning, TEXT("g2"));
	NetMulti_Grab();
}


void AJSH_BetaPlayer::NetMulti_Grab_Implementation()
{
	if (HasAuthority()) UE_LOG(LogTemp, Warning, TEXT("g3"));
	if ( BHasPop )
	{
		if (HasAuthority()) UE_LOG(LogTemp, Warning, TEXT("g4"));
		MyReleasePop();
		PopModeON = false;
	}
	else
	{
		if (HasAuthority()) UE_LOG(LogTemp, Warning, TEXT("g5"));
		MyTakePop();
	}
}




void AJSH_BetaPlayer::MyTakePop()
{
	if (HasAuthority()) UE_LOG(LogTemp, Warning, TEXT("t1"));
	Server_MyTakePop();
}

void AJSH_BetaPlayer::Server_MyTakePop_Implementation()
{
	if (HasAuthority()) UE_LOG(LogTemp, Warning, TEXT("t2"));
	NetMulti_MyTakePop();
}

void AJSH_BetaPlayer::NetMulti_MyTakePop_Implementation()
{
	if (HasAuthority()) UE_LOG(LogTemp, Warning, TEXT("t3"));
	for ( AActor* Pop : PopList )
	{
		if (HasAuthority()) UE_LOG(LogTemp, Warning, TEXT("t3-2"));
		float tempDist = GetDistanceTo(Pop);
		if (HasAuthority()) UE_LOG(LogTemp, Warning, TEXT("t3-3"));
		if ( tempDist > GrabDistance )
			continue;

		// 서버 쪽에서 여기서 자꾸 막힘... 왜 그런지 몰르겠음 ....
		//if (Pop->GetOwner() != nullptr) continue;


		GrabPopActor = Pop;
		if (HasAuthority()) UE_LOG(LogTemp, Warning, TEXT("t4"));
		if (IsLocallyControlled()) UE_LOG(LogTemp, Warning, TEXT("rt4"));
		Pop->SetOwner(this);
		BHasPop = true;

		if (HasAuthority()) UE_LOG(LogTemp, Warning, TEXT("t5"));
		if (IsLocallyControlled()) UE_LOG(LogTemp, Warning, TEXT("rt5"));
		AttachPop(GrabPopActor);
		PopModeON = true;
		// WantWalk = true;
		//
		// if (!GrabPopActor->ActorHasTag(FName("Count")))
		// {
		// 	GrabPopActor->Tags.Add(FName("Count"));
		// }

		break;
	}
}


void AJSH_BetaPlayer::MyReleasePop()
{
	if (HasAuthority()) UE_LOG(LogTemp, Warning, TEXT("r1"));

	Server_MyReleasePop();
}
void AJSH_BetaPlayer::Server_MyReleasePop_Implementation()
{
	if (HasAuthority()) UE_LOG(LogTemp, Warning, TEXT("r2"));
	NetMulti_MyReleasePop();
}


void AJSH_BetaPlayer::NetMulti_MyReleasePop_Implementation()
{

	if ( false == BHasPop) return;
	if (HasAuthority()) UE_LOG(LogTemp, Warning, TEXT("r3"));
	if ( BHasPop )
	{
		BHasPop = false;
		if (HasAuthority()) UE_LOG(LogTemp, Warning, TEXT("r4"));
	}


	if ( GrabPopActor )
	{
		if (HasAuthority()) UE_LOG(LogTemp, Warning, TEXT("r5"));
		DetachPop(GrabPopActor);

		GrabPopActor->SetOwner(nullptr);

		GrabPopActor = nullptr;
	}

	if (HasAuthority()) UE_LOG(LogTemp, Warning, TEXT("r6"));
	PopModeON = false;
}




void AJSH_BetaPlayer::AttachPop(AActor* PopActor)
{
	if (HasAuthority()) UE_LOG(LogTemp, Warning, TEXT("a1"));
	Server_AttachPop(PopActor);
}
void AJSH_BetaPlayer::Server_AttachPop_Implementation(AActor* PopActor)
{
	if (HasAuthority()) UE_LOG(LogTemp, Warning, TEXT("a2"));
	NetMulti_AttachPop(PopActor);
}

void AJSH_BetaPlayer::NetMulti_AttachPop_Implementation(AActor* PopActor)
{
	if (HasAuthority()) UE_LOG(LogTemp, Warning, TEXT("a3"));
	GrabPopActor = PopActor;
	// auto* mesh = GrabPopActor->GetComponentByClass<UStaticMeshComponent>();
	// check(mesh);
	// if ( mesh )
	// {
	// 	if (HasAuthority()) UE_LOG(LogTemp, Warning, TEXT("a3"));
	// 	mesh->SetSimulatePhysics(false);
	// 	mesh->AttachToComponent(HandComp , FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	// }


	CapsuleComp = GrabPopActor->GetComponentByClass<UCapsuleComponent>();
	check(CapsuleComp);
	if ( CapsuleComp )
	{
		if (HasAuthority()) UE_LOG(LogTemp, Warning, TEXT("a3"));
		CapsuleComp->SetSimulatePhysics(false);
		CapsuleComp->AttachToComponent(HandComp , FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	}
}


void AJSH_BetaPlayer::DetachPop(AActor* PopActor)
{
	if (HasAuthority()) UE_LOG(LogTemp, Warning, TEXT("d1"));
	Server_DetachPop(PopActor);

}
void AJSH_BetaPlayer::Server_DetachPop_Implementation(AActor* PopActor)
{
	if (HasAuthority()) UE_LOG(LogTemp, Warning, TEXT("d2"));

	NetMulti_DetachPop(PopActor);
}

void AJSH_BetaPlayer::NetMulti_DetachPop_Implementation(AActor* PopActor)
{
	if (HasAuthority()) UE_LOG(LogTemp, Warning, TEXT("d3"));

	// auto* mesh = PopActor->GetComponentByClass<UStaticMeshComponent>();
	// check(mesh);
	// if ( mesh )
	// {
	// 	if (HasAuthority()) UE_LOG(LogTemp, Warning, TEXT("d4"));
	//
	// 	// 물리를 켜주고싶다.
	// 	mesh->SetSimulatePhysics(true);
	// 	// 분리하고싶다..
	// 	mesh->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	// }

	CapsuleComp = GrabPopActor->GetComponentByClass<UCapsuleComponent>();
	check(CapsuleComp);
	if ( CapsuleComp )
	{
		// 물리를 켜주고싶다.
		CapsuleComp->SetSimulatePhysics(true);
		// 분리하고싶다..
		CapsuleComp->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	}
}
