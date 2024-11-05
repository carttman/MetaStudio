// Fill out your copyright notice in the Description page of Project Settings.


#include "../../JSH/Gizmo/JSH_GizmoX.h"

// Sets default values
AJSH_GizmoX::AJSH_GizmoX()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AJSH_GizmoX::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AJSH_GizmoX::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AJSH_GizmoX::Teleport_to_EditorActor(FRotator TelRotator)
{
	
}



