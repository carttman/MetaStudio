// Fill out your copyright notice in the Description page of Project Settings.


#include "../Gizmo/JSH_Gizmo.h"

#include "JSH_Scale_GizmoX.h"
#include "../Gizmo/JSH_Translate_GizmoBox.h"
#include "../Gizmo/JSH_Translate_GizmoX.h"
#include "../Gizmo/JSH_Translate_GizmoY.h"
#include "../Gizmo/JSH_Translate_GizmoZ.h"
#include "../Gizmo/JSH_Scale_GizmoX.h"
#include "Kismet/GameplayStatics.h"
#include "MetaStudios/JSH/JSH_Editor_SpawnActor.h"
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
		//Origin_Translate_Box = Cast<AJSH_Translate_GizmoBox>(Translate_Box);
	}
	
	Translate_X = CreateDefaultSubobject<UChildActorComponent>(TEXT("Translate_X"));
	Translate_X->SetupAttachment(Translate_Box);
	UClass* Translate_X_Class = LoadObject<UClass>(NULL, TEXT("/Game/JSH/BP/Gizmo/Translate_X/BP_TranslateGizmo_X.BP_TranslateGizmo_X_C"));
	if (Translate_X_Class)
	{
		Translate_X->SetChildActorClass(Translate_X_Class);
		Translate_X->SetRelativeLocationAndRotation(FVector(8.0, 0, 0), FRotator(0, 0, 0));
		Translate_X->SetRelativeScale3D(FVector(1.0, 1.0, 1.0));
		//Origin_Translate_X = Cast<AJSH_Translate_GizmoX>(Translate_X);
	}
	
	
	Translate_Y = CreateDefaultSubobject<UChildActorComponent>(TEXT("Translate_Y"));
	Translate_Y->SetupAttachment(Translate_Box);
	UClass* Translate_Y_Class = LoadObject<UClass>(NULL, TEXT("/Game/JSH/BP/Gizmo/Translate_Y/BP_TranslateGizmo_Y.BP_TranslateGizmo_Y_C"));
	if (Translate_Y_Class)
	{
		Translate_Y->SetChildActorClass(Translate_Y_Class);
		Translate_Y->SetRelativeLocationAndRotation(FVector(0, 8.0, 0), FRotator(0, 90.0, 0));
		Translate_Y->SetRelativeScale3D(FVector(1.0, 1.0, 1.0));
		//Origin_Translate_Y = Cast<AJSH_Translate_GizmoY>(Translate_Y);
	}
	
	Translate_Z = CreateDefaultSubobject<UChildActorComponent>(TEXT("Translate_Z"));
	Translate_Z->SetupAttachment(Translate_Box);
	UClass* Translate_Z_Class = LoadObject<UClass>(NULL, TEXT("/Game/JSH/BP/Gizmo/Translate_Z/BP_TranslateGizmo_Z.BP_TranslateGizmo_Z_C"));
	if (Translate_Z_Class)
	{
		Translate_Z->SetChildActorClass(Translate_Z_Class);
		Translate_Z->SetRelativeLocationAndRotation(FVector(0, 0, 8.0), FRotator(90.0, 0, 0));
		Translate_Z->SetRelativeScale3D(FVector(1.0, 1.0, 1.0));
		//Origin_Translate_Z = Cast<AJSH_Translate_GizmoZ>(Translate_Z);
	}

	// if (OriginPlayer->Editor_SpawnActor->GizmoX_ON)
	// {
	// 	Origin->SetCollisionProfileName(TEXT("NoCollision"));
	// }
	// else if (OriginPlayer->Editor_SpawnActor->GizmoZ_ON)
	// {
	// 	Origin->SetCollisionProfileName(TEXT("NoCollision"));
	// }
	// else
	// {
	// 	Origin->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	// }

	////// Sclae /////
	Scale_Box = CreateDefaultSubobject<UChildActorComponent>(TEXT("Scale_Box"));
	Scale_Box->SetupAttachment(RootScence);
	UClass* BoxClass = LoadObject<UClass>(NULL, TEXT("/Game/JSH/BP/Gizmo/Scale_Box/BP_ScaleBox.BP_ScaleBox_C"));
	if (BoxClass)
	{
		Scale_Box->SetChildActorClass(BoxClass);
		//Scale_Box->SetVisibility(false);
	}

	
	Scale_X = CreateDefaultSubobject<UChildActorComponent>(TEXT("Scale_X"));
	Scale_X->SetupAttachment(Scale_Box);
	UClass* Scale_X_Class = LoadObject<UClass>(NULL, TEXT("/Game/JSH/BP/Gizmo/Scale_X/BP_ScaleGizmo_X.BP_ScaleGizmo_X_C"));
	if (Scale_X_Class)
	{
		Scale_X->SetChildActorClass(Scale_X_Class);
		Scale_X->SetRelativeLocationAndRotation(FVector(8.0, 0, 0), FRotator(0, 0, 0));
		Scale_X->SetRelativeScale3D(FVector(1.0, 1.0, 1.0));
		//Scale_X->SetVisibility(false);
	}
	
	
	Scale_Y = CreateDefaultSubobject<UChildActorComponent>(TEXT("Scale_Y"));
	Scale_Y->SetupAttachment(Scale_Box);
	UClass* Scale_Y_Class = LoadObject<UClass>(NULL, TEXT("/Game/JSH/BP/Gizmo/Scale_Y/BP_ScaleGizmo_Y.BP_ScaleGizmo_Y_C"));
	if (Scale_Y_Class)
	{
		Scale_Y->SetChildActorClass(Scale_Y_Class);
		Scale_Y->SetRelativeLocationAndRotation(FVector(0, 8.0, 0), FRotator(0, 90.0, 0));
		Scale_Y->SetRelativeScale3D(FVector(1.0, 1.0, 1.0));
		//Scale_Y->SetVisibility(false);
	}
	
	Scale_Z = CreateDefaultSubobject<UChildActorComponent>(TEXT("Scale_Z"));
	Scale_Z->SetupAttachment(Scale_Box);
	UClass* Scale_Z_Class = LoadObject<UClass>(NULL, TEXT("/Game/JSH/BP/Gizmo/Scale_X/BP_ScaleGizmo_X.BP_ScaleGizmo_Z_C"));
	if (Scale_Z_Class)
	{
		Scale_Z->SetChildActorClass(Scale_Z_Class);
		Scale_Z->SetRelativeLocationAndRotation(FVector(0, 0, 8.0), FRotator(90.0, 0, 0));
		Scale_Z->SetRelativeScale3D(FVector(1.0, 1.0, 1.0));
		//Scale_Z->SetVisibility(false);
	}
}

// Called when the game starts or when spawned
void AJSH_Gizmo::BeginPlay()
{
	Super::BeginPlay();
	
	//ScaleMode();

	// JPlayerController = Cast<AJSH_PlayerController>(GetWorld()->GetFirstPlayerController());
	// if (JPlayerController)
	// {
	// 	UE_LOG(LogTemp, Error, TEXT("Begin_ScaleX"));
	// }
	// OriginPlayer = Cast<AJSH_Player>(JPlayerController->GetPawn());
	
	// 처음 크기
	// InitialScale = GetActorScale3D();


	//ScaleMode();
	
	
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
			float ScaleFactor = FMath::GetMappedRangeValueClamped(FVector2D(MinDistance, MaxDistance), FVector2D(0.5f, 6.0f), ClampedDistance);
	
			Translate_Box->SetRelativeScale3D(InitialScale * ScaleFactor);
			Scale_Box->SetRelativeScale3D(InitialScale * ScaleFactor);
		}
	}

	if (OriginPlayer && OriginPlayer->Editor_SpawnActor)
	{
	// 	//SetActorTransform(OriginPlayer->Editor_SpawnActor->GetActorTransform());
		SetActorLocation(OriginPlayer->Editor_SpawnActor->GetActorLocation());
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



void AJSH_Gizmo::BeginPlayerContorller(AJSH_PlayerController* temp)
{
	JPlayerController = temp;
	//JPlayerController = Cast<AJSH_PlayerController>(GetWorld()->GetFirstPlayerController());
	OriginPlayer = Cast<AJSH_Player>(JPlayerController->GetPawn());
	if (OriginPlayer)
	{
		OriginPlayer->Save_Gizmo_Parents(this);
	}
	
	//처음 크기
	InitialScale = GetActorScale3D();

	Child_Actor_Detect();

	//AttachToActor(OriginPlayer->Editor_SpawnActor, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
}

void AJSH_Gizmo::Begin_PlayerData(AJSH_Player* temp, AJSH_PlayerController* control)
{
	OriginPlayer = temp;
	if (OriginPlayer)
	{
		OriginPlayer->Save_Gizmo_Parents(this);
	}

	JPlayerController = control;
	
	//처음 크기
	InitialScale = GetActorScale3D();

	Child_Actor_Detect();
	//AttachToActor(OriginPlayer->Editor_SpawnActor, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
}



void AJSH_Gizmo::Child_Actor_Detect()
{
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), TEXT("Tranlate_Gizmo_Box"), Tag_Box);
	if(Tag_Box.Num() > 0)
	{
		Origin_Translate_Box = Cast<AJSH_Translate_GizmoBox>(Tag_Box[0]);
		if (Origin_Translate_Box != nullptr)
		{
			Origin_Translate_Box->BeginPlayer(OriginPlayer, JPlayerController);
		}
	}
	
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), TEXT("Tranlate_Gizmo_X"), Tag_X);
	if(Tag_X.Num() > 0)
	{
		Origin_Translate_X = Cast<AJSH_Translate_GizmoX>(Tag_X[0]);
		if (Origin_Translate_X != nullptr)
		{
			Origin_Translate_X->BeginPlayer(OriginPlayer, JPlayerController);
		}
	}

	UGameplayStatics::GetAllActorsWithTag(GetWorld(), TEXT("Tranlate_Gizmo_Y"), Tag_Y);
	if(Tag_Y.Num() > 0)
	{
		Origin_Translate_Y = Cast<AJSH_Translate_GizmoY>(Tag_Y[0]);
		if (Origin_Translate_Y != nullptr)
		{
			Origin_Translate_Y->BeginPlayer(OriginPlayer, JPlayerController);
		}
	}

	UGameplayStatics::GetAllActorsWithTag(GetWorld(), TEXT("Tranlate_Gizmo_Z"), Tag_Z);
	if(Tag_Z.Num() > 0)
	{
		Origin_Translate_Z = Cast<AJSH_Translate_GizmoZ>(Tag_Z[0]);
		if (Origin_Translate_Z != nullptr)
		{
			Origin_Translate_Z->BeginPlayer(OriginPlayer, JPlayerController);
		}
	}


	// Sclae
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), TEXT("Scale_Gizmo_X"), Tag_SX);
	if(Tag_SX.Num() > 0)
	{
		Origin_Scale_X = Cast<AJSH_Scale_GizmoX>(Tag_SX[0]);
		if (Origin_Scale_X != nullptr)
		{
			Origin_Scale_X->BeginPlayer(OriginPlayer, JPlayerController);
		}
	}

	//UGameplayStatics::GetAllActorsWithTag(GetWorld(), TEXT("Tranlate_Gizmo_Y"), Tag_Y);
}


