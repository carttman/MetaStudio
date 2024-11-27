// Fill out your copyright notice in the Description page of Project Settings.


#include "JYS/CarPawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputSubsystems.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputComponent.h"
#include "../../../../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraComponent.h"
#include "../../../../Plugins/FX/Niagara/Source/Niagara/Classes/NiagaraSystem.h"
#include "../../../../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraFunctionLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Components/ArrowComponent.h"
#include "JYS/MetaStudiosCharacter.h"
#include "Components/BoxComponent.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"



// Sets default values
ACarPawn::ACarPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DefaultScene = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneComp"));
	SetRootComponent(DefaultScene);

	CarMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CarMesh"));
	CarMesh->SetupAttachment(DefaultScene);


	//SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	//SpringArm->SetupAttachment(CarMesh);
	//SpringArm->TargetArmLength = 300.0f;
	//
	//CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	//CameraComp->SetupAttachment(RootComponent);

	ThrusterComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ThrusterComponent"));
	ThrusterComponent->SetupAttachment(CarMesh);
	ThrusterComponent2 = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ThrusterComponent2"));
	ThrusterComponent2->SetupAttachment(CarMesh);

	RidingPlayer = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("playerVisible"));
	RidingPlayer->SetupAttachment(CarMesh);

	LineTraceArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("LineTraceArrow"));
	LineTraceArrow->SetupAttachment(RootComponent);

	UIGuideBox = CreateDefaultSubobject<UBoxComponent>(TEXT("UIGuideBox"));
	UIGuideBox->SetupAttachment(CarMesh);

	UIGuideBox->SetGenerateOverlapEvents(true);
	UIGuideBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
}

// Called when the game starts or when spawned
void ACarPawn::BeginPlay()
{
	Super::BeginPlay();

	ActivateThruster(false);

	if (UIGuideBox)
	{
		UIGuideBox->OnComponentBeginOverlap.AddDynamic(this, &ACarPawn::OnUIBoxBeginOverlap);
		UIGuideBox->OnComponentEndOverlap.AddDynamic(this, &ACarPawn::OnUIBoxEndOverlap);
	}

	//UGameplayStatics::GetAllActorsWithTag(GetWorld(), TEXT("TTT"), PlayerBox);
	// //********Player 여러명이니깐 루프 돌려서 각자 넣어주기********
	//if (PlayerBox.Num() > 0)
	//{
	//	player =  Cast<AMetaStudiosCharacter>(PlayerBox[0]);
	//	AActor* A_This = <AActor>(this);
	// 
	//	if (player)
	//	{
	// 	//******* 다른곳에서 해보니깐 정상적으로 들어가는걸 봐선 this를 aactor*로 변환해서 생기는 문제인듯
	//  // 1. 안쪽에서 this말고 다른 방식으로 자신을 캐스팅해서 넣어서 넘겨주거나 or player->Addvehicle 각자 나누기 ********
	//  // 1번 방법 이 가장 효율적일 듯 (내 쪽에서 할 수 있는 부분도 맞는디 , 아 그 앞쪽 bp를 코드로 못 옮기겠써 ㅠㅅㅠ)
	//		player->AddVehicle(A_This);
	//	}
	//}
}

// Called every frame
void ACarPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MoveStop == false)
	{
		///////////////LineTrace//////////////////////
		//FVector startLocation = GetActorLocation();
		FVector startLocation = LineTraceArrow->GetComponentLocation();
		//FVector endLocation = startLocation + -(GetActorUpVector() * 10000.0f); // Trace downwards
		FVector endLocation = startLocation + (LineTraceArrow->GetForwardVector() * 10000.0f); // Trace downwards
		FHitResult hitResult;
		FCollisionQueryParams queryParams;
		queryParams.AddIgnoredActor(this);

		float SphereRadius = 100.0f;
		float HalfSphereRadius = SphereRadius * 0.5f;

		//DrawDebugLine(GetWorld(), startLocation, endLocation, FColor::Magenta, false, 1.0f, 0, 20.0f);      

		bool HitDetected = GetWorld()->SweepSingleByChannel(hitResult, startLocation, endLocation, FQuat::Identity, ECC_Visibility, FCollisionShape::MakeSphere(SphereRadius), queryParams);
		if (HitDetected)
		{
			FVector locCharacter = GetActorLocation();
			FVector newLocation = hitResult.ImpactPoint;
			newLocation.Z += HalfSphereRadius;
			locCharacter.Z = newLocation.Z + HalfSphereRadius;
			SetActorLocation(locCharacter);
		}

		FVector CapsuleOrigin = startLocation + (endLocation - startLocation) * 0.5f;
		FColor DrawColor = HitDetected ? FColor::Green : FColor::Red;

		DrawDebugCapsule(GetWorld(), hitResult.ImpactPoint, HalfSphereRadius, SphereRadius, FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(), DrawColor, false, 5.0f);


		//DrawDebugLine(GetWorld(), startLocation, endLocation, FColor::Magenta, false, 1.0f, 0, 20.0f);
		/*
		if (GetWorld()->LineTraceSingleByChannel(hitResult, startLocation, endLocation, ECC_Visibility, queryParams))
		{
			// DrawDebugLine(GetWorld(), startLocation, endLocation, FColor::Magenta, false, 1.0f, 0, 20.0f);

			FVector locCharacter = GetActorLocation();
			FVector newLocation = hitResult.ImpactPoint;
			newLocation.Z += 50.0f;
			locCharacter.Z = newLocation.Z + 50.0f;
			SetActorLocation(locCharacter);

			//FRotator WorldRot = GetActorRotation();
			//
			//FRotator right = UKismetMathLibrary::MakeRotFromYZ(GetActorRightVector(), hitResult.ImpactNormal);
			//
			//FRotator forward = UKismetMathLibrary::MakeRotFromXZ(GetActorForwardVector(), hitResult.ImpactNormal);
			//
			//FRotator NewRot = UKismetMathLibrary::MakeRotator(forward.Roll, right.Pitch, WorldRot.Yaw);
			//
			//SetActorRotation(UKismetMathLibrary::RInterpTo(WorldRot, NewRot, DeltaTime, 1));

			//FVector dir = FVector(0.0f, )
			//FTransform FTform = FTransform(GetControlRotation());
			//FTform.TransformVector(direction);
			//
			//FTransform ttt = FTransform(GetControlRotation());
			//direction =
			//direction.Z = 0;
			//direction.Normalize();


			float PosZ = GetActorLocation().Z;
			UE_LOG(LogTemp, Warning, TEXT("Ground hit : %f : %f"), hitResult.ImpactPoint.Z, PosZ);
		}
		*/
	}

	ActivateThruster(!MoveStop);
	if (MoveStop == true)
	{
		ApplyRollBack();
		ActivateThruster(false);
	}

}

// Called to bind functionality to input
void ACarPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			if (IsLocallyControlled())
			{
				// 매핑이 위로 쌓이기 떄문에 키가 겹쳐서 안될 수 있음 그래서 매핑콘테스트를 클리어해주고 AddMappingContext 해줘야함
				Subsystem->ClearAllMappings();
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
			}
		}
	}

	PlayerInputComponent->BindAction("ExitCar", IE_Pressed, this, &ACarPawn::ExitCar);


	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(CarLook, ETriggerEvent::Triggered, this, &ACarPawn::OnMyActionLook);
		EnhancedInputComponent->BindAction(CarMove, ETriggerEvent::Triggered, this, &ACarPawn::OnMyActionMove);
		EnhancedInputComponent->BindAction(CarMove, ETriggerEvent::Completed, this, &ACarPawn::OnMyActionMove);
	}

}

void ACarPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACarPawn, MoveStop);
}

bool ACarPawn::CanPlayerEnterCar(AMetaStudiosCharacter* targetCharacter)
{
	if (targetCharacter == nullptr)	return false;

	float distance = GetDistanceTo(targetCharacter);
	return distance <= 1000.0f;
}

void ACarPawn::OnMyActionMove(const FInputActionValue& value)
{

	FVector2D v = value.Get<FVector2D>();
	if (v.X <= 0.0f && MoveStop == false)
	{
		//MoveStop = true;
		Server_OnMyActionMove(true);
		return;
	}

	if (v.X <= 0.0f)	return;

	//MoveStop = false;
	direction.X = v.X;

	if (direction.X != 0.0f)
	{
		ApplyRoll(v.Y);
		// UE_LOG(LogTemp, Warning, TEXT("direction.X != 0.0f"))
	}
	else
	{
		ApplyRollBack();
		// UE_LOG(LogTemp, Warning, TEXT("ApplyRollBack"))
	}

	FTransform ttt = FTransform(GetControlRotation());
	direction = ttt.TransformVector(direction);
	direction.Z = 0;
	direction.Normalize();

	AddMovementInput(direction);
	direction = FVector::ZeroVector;

	Server_OnMyActionMove(false);

	Server_UpdateTransform(GetActorLocation(), GetActorRotation());

}

void ACarPawn::Server_OnMyActionMove_Implementation(bool bMove)
{
	MoveStop = bMove;
}

void ACarPawn::OnMyActionLook(const FInputActionValue& value)
{
	FVector2D v = value.Get<FVector2D>();
	//if(v.X != 0.0f )
	//{
	//	//ApplyRoll(v.X * 0.5f);
	//}
}

// 가까이 가면 키 관련 UI 생성
void ACarPawn::OnUIBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//if (OtherActor && OtherActor->IsA(AMetaStudiosCharacter::StaticClass()))
	//{
	//	if (EnterWidgetClass && !ActiveWidget)
	//	{
	//		ActiveWidget = CreateWidget<UUserWidget>(GetWorld(), EnterWidgetClass);
	//		if (ActiveWidget)
	//		{
	//			ActiveWidget->AddToViewport();
	//		}
	//	}
	//}
	if (HasAuthority())	return;
	if ( bExistRider == true )	return;

	if (OtherActor && OtherActor->IsA(AMetaStudiosCharacter::StaticClass()))
	{
		AMetaStudiosCharacter* targetChar = Cast<AMetaStudiosCharacter>(OtherActor);
		if (targetChar->IsLocallyPlayer() == false)	return;

		if (EnterWidgetClass && !ActiveWidget)
		{
			ActiveWidget = CreateWidget<UUserWidget>(GetWorld(), EnterWidgetClass);
			if (ActiveWidget)
			{
				ActiveWidget->AddToViewport();
			}
		}

		if (ActiveWidget)
		{
			ActiveWidget->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

void ACarPawn::OnUIBoxEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (HasAuthority())	return;
	if (bExistRider == true)	return;

	if (OtherActor && OtherActor->IsA(AMetaStudiosCharacter::StaticClass()))
	{
		if (ActiveWidget)
		{
			ActiveWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}
// 가까이 가면 키 관련 UI 생성

void ACarPawn::ResetEnhancedInputSetting(APlayerController* pc)
{
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			if (IsLocallyControlled())
			{
				// 매핑이 위로 쌓이기 떄문에 키가 겹쳐서 안될 수 있음 그래서 매핑콘테스트를 클리어해주고 AddMappingContext 해줘야함
				Subsystem->ClearAllMappings();
				Subsystem->AddMappingContext(DefaultMappingContext, 0);

			}
		}
	}

}

void ACarPawn::ApplyRoll(float RollInput)
{
	FRotator CurrentRotation = GetControlRotation();

	float RollAmount = RollInput * 45.0f;
	float YawAmount = RollInput * 45.0f;

	CurrentRotation.Roll = FMath::FInterpTo(CurrentRotation.Roll, RollAmount, GetWorld()->GetDeltaSeconds(), 3.0f);

	float YawCur = CurrentRotation.Yaw;

	// P = P0 + vt
	CurrentRotation.Yaw = YawCur + 50.0f * RollInput * GetWorld()->DeltaTimeSeconds;

	GetController()->SetControlRotation(CurrentRotation);
}

void ACarPawn::ApplyRollBack()
{
	FRotator CurrentRotation = GetControlRotation();

	float RollAmount = CurrentRotation.Roll;
	//float YawAmount = CurrentRotation.Yaw;

	if (RollAmount == 0.0f /*&& YawAmount == 0.0f*/)	return;

	//CurrentRotation.Yaw = FMath::FInterpTo(CurrentRotation.Yaw, 0.0f, GetWorld()->GetDeltaSeconds(), 2.0f);
	CurrentRotation.Roll = FMath::FInterpTo(CurrentRotation.Roll, 0.0f, GetWorld()->GetDeltaSeconds(), 3.0f);

	GetController()->SetControlRotation(CurrentRotation);
}

void ACarPawn::ActivateThruster(bool bActive)
{
	if (ActiveThruster == bActive)	return;
	ActiveThruster = bActive;

	if (ThrusterComponent)
	{
		if (bActive)
		{
			ThrusterComponent->Activate();
		}
		else
		{
			ThrusterComponent->Deactivate();
		}
	}

	if (ThrusterComponent2)
	{
		if (bActive)
		{
			ThrusterComponent2->Activate();
		}
		else
		{
			ThrusterComponent2->Deactivate();
		}
	}
}

void ACarPawn::SetActivateGuideUI(bool bActive)
{
	if (UIGuideBox == nullptr)	return;
	
	if (bActive == true)
	{
		UIGuideBox->OnComponentBeginOverlap.AddDynamic(this, &ACarPawn::OnUIBoxBeginOverlap);
		UIGuideBox->OnComponentEndOverlap.AddDynamic(this, &ACarPawn::OnUIBoxEndOverlap);
	}
	else
	{
		UIGuideBox->OnComponentBeginOverlap.RemoveDynamic(this, &ACarPawn::OnUIBoxBeginOverlap);
		UIGuideBox->OnComponentEndOverlap.RemoveDynamic(this, &ACarPawn::OnUIBoxEndOverlap);
	}		
}

void ACarPawn::Server_UpdateTransform_Implementation(FVector newLocation, FRotator newRotation)
{
	SetActorLocationAndRotation(newLocation, newRotation);
}

void ACarPawn::ExitCar()
{
	if (IsLocallyControlled())
	{
		Server_ExitCar();
	}

	//if (ExitWidgetClass && ActiveWidget)
	//{
	//	ActiveWidget = CreateWidget<UUserWidget>(GetWorld(), ExitWidgetClass);
	//	if (ActiveWidget)
	//	{
	//		ActiveWidget->AddToViewport();
	//	}
	//}
	//else
	//{
	//	ActiveWidget = CreateWidget<UUserWidget>(GetWorld(), ExitWidgetClass);
	//	if (!ActiveWidget)
	//	{
	//		ActiveWidget->RemoveFromParent();
	//	}
	//}
}

void ACarPawn::Server_ExitCar_Implementation()
{
	if (player == nullptr)	return;

	FVector carLoc = GetActorLocation();
	FRotator carRot = GetActorRotation();

	FVector offset = carRot.RotateVector(FVector(200.0f, 200.0f, 0.0f));
	FVector playerSpawnLocation = carLoc + offset;

	player->SetActorLocation(playerSpawnLocation);
	player->SetActorRelativeRotation(carRot);

	GetController()->Possess(player);


	FRotator rot = this->GetActorRotation();
	rot.Roll = 0.0f;
	this->SetActorRotation(rot);

	NetMulticast_ExitCar();

	bExistRider = false;
}

void ACarPawn::NetMulticast_ExitCar_Implementation()
{
	if (player)
	{
		FVector carLoc = GetActorLocation();
		FRotator carRot = GetActorRotation();

		FVector offset = carRot.RotateVector(FVector(-100.0f, 100.0f, 10.0f));
		FVector playerSpawnLocation = carLoc + offset;

		player->SetActorLocation(playerSpawnLocation);


		/// 여기서 튕김
		//if (HasAuthority())
		//{
		//	player->SetActorRotation(carRot);
		//	UE_LOG(LogTemp, Warning, TEXT("player->SetActorRotation(carRot)"))
		//}
		//else 
		//{
		//	UE_LOG(LogTemp, Error, TEXT("!!!!player->SetActorRotation(carRot)"))
		//}
		// player->SetActorRotation(carRot);


		//characterController->`(player);
		player->GetMesh()->SetVisibility(true);
		player->JetMesh->SetVisibility(true);

		RidingPlayer->SetVisibility(false);
		bExistRider = false;
		if (IsLocallyControlled())
		{
			player->ResetEnhancedInputSetting(Cast<APlayerController>(GetWorld()->GetFirstPlayerController()));

			// 내렸을 때 오토바이의 Rotation이 0.0f
			AController* controller = GetController();
			if (controller)
			{
				FRotator CurrentRotation = GetControlRotation();
				if (CurrentRotation.Roll != 0.0f)
				{
					CurrentRotation.Roll = 0.0f;
					controller->SetControlRotation(CurrentRotation);
				}
			}
		}

		//FRotator CurrentRotation = GetControlRotation();

		//float RollAmount = CurrentRotation.Roll;
		//if (RollAmount == 0.0f)	return;

		//CurrentRotation.Roll = 0.0f;

		//GetController()->SetControlRotation(CurrentRotation);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Player reference is null in NetMulticast_ExitCar_Implementation."));
	}
}



