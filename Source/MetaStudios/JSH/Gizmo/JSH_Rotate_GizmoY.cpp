// Fill out your copyright notice in the Description page of Project Settings.


#include "../Gizmo/JSH_Rotate_GizmoY.h"
#include "MetaStudios/JSH/JSH_PlayerController.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "Components/PrimitiveComponent.h"
#include "DrawDebugHelpers.h"
#include "JSH_Translate_GizmoX.h"
#include "JSH_Translate_GizmoY.h"
#include "JSH_Translate_GizmoZ.h"
#include "JSH_Translate_GizmoBox.h"
#include "JSH_Scale_GizmoBox.h"
#include "JSH_Scale_GizmoY.h"
#include "JSH_Scale_GizmoZ.h"
#include "JSH_Scale_GizmoX.h"
#include "JSH_Rotate_GizmoX.h"
#include "Engine/EngineTypes.h" 
#include "MetaStudios/JSH/JSH_Editor_SpawnActor.h"

// Sets default values
AJSH_Rotate_GizmoY::AJSH_Rotate_GizmoY()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AJSH_Rotate_GizmoY::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AJSH_Rotate_GizmoY::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

