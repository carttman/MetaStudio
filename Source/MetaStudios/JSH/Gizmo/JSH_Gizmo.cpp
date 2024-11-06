// Fill out your copyright notice in the Description page of Project Settings.


#include "../Gizmo/JSH_Gizmo.h"

// Sets default values
AJSH_Gizmo::AJSH_Gizmo()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	

	RootScence = CreateDefaultSubobject<USceneComponent>(TEXT("RootScence"));
	RootComponent = RootScence;

	
	Scale_Box = CreateDefaultSubobject<UChildActorComponent>(TEXT("Scale_Box"));
	Scale_Box->SetupAttachment(RootScence);
	// Scale_Box->SetChildActorClass(AJSH_Scale_GizmoX::StaticClass()); 
	UClass* BoxClass = LoadObject<UClass>(NULL, TEXT("/Game/JSH/BP/Gizmo/ScaleBox/BP_ScaleBox.BP_ScaleBox_C"));
	if (BoxClass)
	{
		Scale_Box->SetChildActorClass(BoxClass);
	}


	//(X=8.000000,Y=0.000000,Z=0.000000)
	
	Scale_X = CreateDefaultSubobject<UChildActorComponent>(TEXT("Scale_X"));
	Scale_X->SetupAttachment(Scale_Box);
	UClass* Scale_X_Class = LoadObject<UClass>(NULL, TEXT("/Game/JSH/BP/Gizmo/X/BP_ScaleGizmo_X.BP_ScaleGizmo_X_C"));
	if (Scale_X_Class)
	{
		Scale_X->SetChildActorClass(Scale_X_Class);
		Scale_X->SetRelativeLocationAndRotation(FVector(8.0, 0, 0), FRotator(0, 0, 0));
		Scale_X->SetRelativeScale3D(FVector(1.0, 1.0, 1.0));
	}
	
	
	Scale_Y = CreateDefaultSubobject<UChildActorComponent>(TEXT("Scale_Y"));
	Scale_Y->SetupAttachment(Scale_Box);
	UClass* Scale_Y_Class = LoadObject<UClass>(NULL, TEXT("/Game/JSH/BP/Gizmo/Y/BP_ScaleGizmo_Y.BP_ScaleGizmo_Y_C"));
	if (Scale_Y_Class)
	{
		Scale_Y->SetChildActorClass(Scale_Y_Class);
		Scale_Y->SetRelativeLocationAndRotation(FVector(0, 8.0, 0), FRotator(0, 90.0, 0));
		Scale_Y->SetRelativeScale3D(FVector(1.0, 1.0, 1.0));
	}
	
	Scale_Z = CreateDefaultSubobject<UChildActorComponent>(TEXT("Scale_Z"));
	Scale_Z->SetupAttachment(Scale_Box);
	UClass* Scale_Z_Class = LoadObject<UClass>(NULL, TEXT("/Game/JSH/BP/Gizmo/Z/BP_ScaleGizmo_Z.BP_ScaleGizmo_Z_C"));
	if (Scale_Z_Class)
	{
		Scale_Z->SetChildActorClass(Scale_Z_Class);
		Scale_Z->SetRelativeLocationAndRotation(FVector(0, 0, 8.0), FRotator(90.0, 0, 0));
		Scale_Z->SetRelativeScale3D(FVector(1.0, 1.0, 1.0));
	}
}

// Called when the game starts or when spawned
void AJSH_Gizmo::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AJSH_Gizmo::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

