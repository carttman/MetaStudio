// Fill out your copyright notice in the Description page of Project Settings.


#include "../Gizmo/JSH_Gizmo.h"

#include "MetaStudios/JSH/JSH_PlayerController.h"

// Sets default values
AJSH_Gizmo::AJSH_Gizmo()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	
	RootScence = CreateDefaultSubobject<USceneComponent>(TEXT("RootScence"));
	RootComponent = RootScence;

	
	/// Translate

	// Translate_RootScence = CreateDefaultSubobject<USceneComponent>(TEXT("Translate_RootScence"));
	// Translate_RootScence->SetupAttachment(RootScence);
	
	Translate_Box = CreateDefaultSubobject<UChildActorComponent>(TEXT("Translate_Box"));
	Translate_Box->SetupAttachment(RootScence);
	UClass* TranslateBoxClass = LoadObject<UClass>(NULL, TEXT("/Game/JSH/BP/Gizmo/Translate_Box/BP_TranslateBox.BP_TranslateBox_C"));
	if (TranslateBoxClass)
	{
		Translate_Box->SetChildActorClass(TranslateBoxClass);
		Translate_Box->SetRelativeLocationAndRotation(FVector(0, 0, 0), FRotator(0, 0, 0));
		Translate_Box->SetRelativeScale3D(FVector(1.0, 1.0, 1.0));
	}

	
	Translate_X = CreateDefaultSubobject<UChildActorComponent>(TEXT("Translate_X"));
	Translate_X->SetupAttachment(Translate_Box);
	UClass* Translate_X_Class = LoadObject<UClass>(NULL, TEXT("/Game/JSH/BP/Gizmo/Translate_X/BP_TranslateGizmo_X.BP_TranslateGizmo_X_C"));
	if (Translate_X_Class)
	{
		Translate_X->SetChildActorClass(Translate_X_Class);
		Translate_X->SetRelativeLocationAndRotation(FVector(8.0, 0, 0), FRotator(0, 0, 0));
		Translate_X->SetRelativeScale3D(FVector(1.0, 1.0, 1.0));
	}
	
	
	Translate_Y = CreateDefaultSubobject<UChildActorComponent>(TEXT("Translate_Y"));
	Translate_Y->SetupAttachment(Translate_Box);
	UClass* Translate_Y_Class = LoadObject<UClass>(NULL, TEXT("/Game/JSH/BP/Gizmo/Translate_Y/BP_TranslateGizmo_Y.BP_TranslateGizmo_Y_C"));
	if (Translate_Y_Class)
	{
		Translate_Y->SetChildActorClass(Translate_Y_Class);
		Translate_Y->SetRelativeLocationAndRotation(FVector(0, 8.0, 0), FRotator(0, 90.0, 0));
		Translate_Y->SetRelativeScale3D(FVector(1.0, 1.0, 1.0));
	}
	
	Translate_Z = CreateDefaultSubobject<UChildActorComponent>(TEXT("Translate_Z"));
	Translate_Z->SetupAttachment(Translate_Box);
	UClass* Translate_Z_Class = LoadObject<UClass>(NULL, TEXT("/Game/JSH/BP/Gizmo/Translate_Z/BP_TranslateGizmo_Z.BP_TranslateGizmo_Z_C"));
	if (Translate_Z_Class)
	{
		Translate_Z->SetChildActorClass(Translate_Z_Class);
		Translate_Z->SetRelativeLocationAndRotation(FVector(0, 0, 8.0), FRotator(90.0, 0, 0));
		Translate_Z->SetRelativeScale3D(FVector(1.0, 1.0, 1.0));
	}



	////// Sclae /////
	Scale_Box = CreateDefaultSubobject<UChildActorComponent>(TEXT("Scale_Box"));
	Scale_Box->SetupAttachment(RootScence);
	// Scale_Box->SetChildActorClass(AJSH_Scale_GizmoX::StaticClass()); 
	UClass* BoxClass = LoadObject<UClass>(NULL, TEXT("/Game/JSH/BP/Gizmo/Scale_Box/BP_ScaleBox.BP_ScaleBox_C"));
	if (BoxClass)
	{
		Scale_Box->SetChildActorClass(BoxClass);
		Scale_Box->SetVisibility(false);
	}

	
	Scale_X = CreateDefaultSubobject<UChildActorComponent>(TEXT("Scale_X"));
	Scale_X->SetupAttachment(Scale_Box);
	UClass* Scale_X_Class = LoadObject<UClass>(NULL, TEXT("/Game/JSH/BP/Gizmo/Scale_X/BP_ScaleGizmo_X.BP_ScaleGizmo_X_C"));
	if (Scale_X_Class)
	{
		Scale_X->SetChildActorClass(Scale_X_Class);
		Scale_X->SetRelativeLocationAndRotation(FVector(8.0, 0, 0), FRotator(0, 0, 0));
		Scale_X->SetRelativeScale3D(FVector(1.0, 1.0, 1.0));
		Scale_X->SetVisibility(false);
	}
	
	
	Scale_Y = CreateDefaultSubobject<UChildActorComponent>(TEXT("Scale_Y"));
	Scale_Y->SetupAttachment(Scale_Box);
	UClass* Scale_Y_Class = LoadObject<UClass>(NULL, TEXT("/Game/JSH/BP/Gizmo/Scale_X/BP_ScaleGizmo_X.BP_ScaleGizmo_X_C"));
	if (Scale_Y_Class)
	{
		Scale_Y->SetChildActorClass(Scale_Y_Class);
		Scale_Y->SetRelativeLocationAndRotation(FVector(0, 8.0, 0), FRotator(0, 90.0, 0));
		Scale_Y->SetRelativeScale3D(FVector(1.0, 1.0, 1.0));
		Scale_Y->SetVisibility(false);
	}
	
	Scale_Z = CreateDefaultSubobject<UChildActorComponent>(TEXT("Scale_Z"));
	Scale_Z->SetupAttachment(Scale_Box);
	UClass* Scale_Z_Class = LoadObject<UClass>(NULL, TEXT("/Game/JSH/BP/Gizmo/Scale_X/BP_ScaleGizmo_X.BP_ScaleGizmo_X_C"));
	if (Scale_Z_Class)
	{
		Scale_Z->SetChildActorClass(Scale_Z_Class);
		Scale_Z->SetRelativeLocationAndRotation(FVector(0, 0, 8.0), FRotator(90.0, 0, 0));
		Scale_Z->SetRelativeScale3D(FVector(1.0, 1.0, 1.0));
		Scale_Z->SetVisibility(false);
	}
}

// Called when the game starts or when spawned
void AJSH_Gizmo::BeginPlay()
{
	Super::BeginPlay();
	
	//ScaleMode();

	JPlayerController = Cast<AJSH_PlayerController>(GetWorld()->GetFirstPlayerController());
	if (JPlayerController)
	{
		UE_LOG(LogTemp, Error, TEXT("Begin_ScaleX"));
	}

	InitialScale = GetActorScale3D();
}

// Called every frame
void AJSH_Gizmo::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
	if (JPlayerController)
	{
		APawn* PlayerPawn = JPlayerController->GetPawn();
		if (PlayerPawn)
		{
			float DistanceToPlayer = FVector::Distance(GetActorLocation(), PlayerPawn->GetActorLocation());
			float ClampedDistance = FMath::Clamp(DistanceToPlayer, MinDistance, MaxDistance);
			float ScaleFactor = FMath::GetMappedRangeValueClamped(FVector2D(MinDistance, MaxDistance), FVector2D(0.1f, 6.0f), ClampedDistance);
	
			Translate_Box->SetRelativeScale3D(InitialScale * ScaleFactor);
			// Translate_X->SetRelativeScale3D(InitialScale * ScaleFactor);
			// Translate_Y->SetRelativeScale3D(InitialScale * ScaleFactor);
			// Translate_Z->SetRelativeScale3D(InitialScale * ScaleFactor);
			
			//SetActorScale3D(InitialScale * ScaleFactor);
	
		}
	}
	
	if (Clicked)
	{
		NotifyActorOnClicked();
	}

}


#pragma region  Mode
void AJSH_Gizmo::TranslateMode()
{
	// Translate On
	Translate_Box->SetVisibility(true);
	Translate_X->SetVisibility(true);
	Translate_Y->SetVisibility(true);
	Translate_Z->SetVisibility(true);
	
	// Scale Off
	Scale_Box->SetVisibility(false);
	Scale_X->SetVisibility(false);
	Scale_Y->SetVisibility(false);
	Scale_Z->SetVisibility(false);
}



void AJSH_Gizmo::ScaleMode()
{

	// Translate Off
	Translate_Box->SetVisibility(false);
	Translate_X->SetVisibility(false);
	Translate_Y->SetVisibility(false);
	Translate_Z->SetVisibility(false);
	
	// Scale On
	Scale_Box->SetVisibility(true);
	Scale_X->SetVisibility(true);
	Scale_Y->SetVisibility(true);
	Scale_Z->SetVisibility(true);
}



#pragma endregion



#pragma region Ing

void AJSH_Gizmo::NotifyActorOnClicked(FKey ButtonPressed)
{
	Super::NotifyActorOnClicked(ButtonPressed);
	
	// UE_LOG(LogTemp, Error, TEXT("bbbb"));
	//
	//
	// // 마우스 2D -> 3D Vector 변환
	// if (JPlayerController->GetMousePosition(MousePosition.X, MousePosition.Y))
	// {
	// 	JPlayerController->DeprojectMousePositionToWorld(Mouse_WorldLocation, Mouse_WorldDirection);
	// }
	//
	//
	// // UE_LOG(LogTemp, Error, TEXT("2d %s"), *MousePosition.ToString());
	// // UE_LOG(LogTemp, Error, TEXT("3d %s"), *Mouse_WorldLocation.ToString());
	// FVector Start = Mouse_WorldLocation;
	// FVector End =  (Mouse_WorldDirection * 10000.0f) + Mouse_WorldLocation;
	//
	// FHitResult HitResult;
	// FCollisionQueryParams Params;
	// //Params.AddIgnoredActor(this);
	//
	//
	//
	// bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);
	// //bool bHit = GetWorld()->linetrace(HitResult, Start, End, ECC_Visibility, Params);
	// if (bHit)
	// {
	// 	//DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 1, 0, 0.3);
	// 	
	// 	if (!firstclick && Clicked == false)
	// 	{
	// 		if (!Clicked)
	// 		{
	// 			Clicked = true;
	// 		}
	// 		
	// 		firstclick = true;
	// 		// 처음 마우스 위치 저장
	// 		// Start_Mouse_WorldLocation = HitResult.Location.X;
	// 		
	// 		StartMouselocation = HitResult.ImpactPoint;
	// 		StartGizmoLocation = this->GetActorLocation();
	// 		StartActor_Location = StartMouselocation - StartGizmoLocation;
	// 		float GapX = StartMouselocation.X - StartGizmoLocation.X;
	// 		UE_LOG(LogTemp, Error, TEXT("point %s"), *HitResult.ImpactPoint.ToString());
	// 		UE_LOG(LogTemp, Error, TEXT("gizmo %s"), *StartGizmoLocation.ToString());
	// 	}
	// 	else
	// 	{
	// 		End_Location = HitResult.ImpactPoint;
	// 		FVector see = StartMouselocation - End_Location;
	// 		
	// 		FVector NewLocation = FVector(End_Location.X - StartActor_Location.X, StartGizmoLocation.Y, StartGizmoLocation.Z);
	// 		this->SetActorLocation(NewLocation);
	// 		
	// 		
	// 		firstclick = false;
	// 	}
	// }
	// else
	// {
	// 	DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 1, 0, 0.3);
	// }
}



void AJSH_Gizmo::NotifyActorOnReleased(FKey ButtonReleased)
{
	Super::NotifyActorOnReleased(ButtonReleased);

	Clicked = false;
}

void AJSH_Gizmo::OriginColor()
{
}

void AJSH_Gizmo::SelectedColor()
{
}

#pragma endregion
