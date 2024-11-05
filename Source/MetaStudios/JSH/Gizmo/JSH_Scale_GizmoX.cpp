// Fill out your copyright notice in the Description page of Project Settings.


#include "../../JSH/Gizmo/JSH_Scale_GizmoX.h"
#include "MetaStudios/JSH/JSH_PlayerController.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "Components/PrimitiveComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/EngineTypes.h" 


// Sets default values
AJSH_Scale_GizmoX::AJSH_Scale_GizmoX()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 기본 색상
	Origin = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Origin"));
	RootComponent = Origin;
	ConstructorHelpers::FObjectFinder<UStaticMesh> TMesh(TEXT("/Script/Engine.StaticMesh'/Game/JSH/BP/Gizmo/X/SM_ScaleHandle_XX.SM_ScaleHandle_XX'"));
	if (TMesh.Succeeded())
	{
		Origin->SetStaticMesh(TMesh.Object);

	}

	// 선택되었을 때 노란색
	Selected = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Selected"));
	Selected->SetupAttachment(RootComponent);
	ConstructorHelpers::FObjectFinder<UStaticMesh> TMesh2(TEXT("/Script/Engine.StaticMesh'/Game/JSH/BP/Gizmo/Select/SM_ScaleHandle_Select.SM_ScaleHandle_Select'"));
	if (TMesh2.Succeeded())
	{
		Selected->SetStaticMesh(TMesh2.Object);
		Selected->SetVisibility(false);
		//Origin->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, -90, 0));
		//Origin->SetRelativeScale3D(FVector(0.5, 0.5, 0.5));
	}
}

// Called when the game starts or when spawned
void AJSH_Scale_GizmoX::BeginPlay()
{
	Super::BeginPlay();

	JPlayerController = Cast<AJSH_PlayerController>(GetWorld()->GetFirstPlayerController());
	if (JPlayerController)
	{
		UE_LOG(LogTemp, Error, TEXT("Begin_ScaleX"));
	}
}

// Called every frame
void AJSH_Scale_GizmoX::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void AJSH_Scale_GizmoX::NotifyActorOnClicked(FKey ButtonPressed)
{
	Super::NotifyActorOnClicked(ButtonPressed);
	UE_LOG(LogTemp, Error, TEXT("Norify_Click"));
}



// 오버랩 색상 변경
void AJSH_Scale_GizmoX::NotifyActorBeginCursorOver()
{
	Super::NotifyActorBeginCursorOver();

	Selected->SetVisibility(true);
	Origin->SetVisibility(false);
}
void AJSH_Scale_GizmoX::NotifyActorEndCursorOver()
{
	Super::NotifyActorEndCursorOver();

	Selected->SetVisibility(false);
	Origin->SetVisibility(true);
}
	