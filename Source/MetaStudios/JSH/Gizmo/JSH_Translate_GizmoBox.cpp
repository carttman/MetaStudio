// Fill out your copyright notice in the Description page of Project Settings.


#include "../JSH/Gizmo/JSH_Translate_GizmoBox.h"
#include "MetaStudios/JSH/JSH_PlayerController.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "Components/PrimitiveComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/EngineTypes.h" 
#include "MetaStudios/JSH/JSH_Editor_SpawnActor.h"

// Sets default values
AJSH_Translate_GizmoBox::AJSH_Translate_GizmoBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 기본 색상
	Origin = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Origin"));
	RootComponent = Origin;
	ConstructorHelpers::FObjectFinder<UStaticMesh> TMesh(TEXT("/Script/Engine.StaticMesh'/Game/JSH/BP/Gizmo/StaticMesh/SM_ScreenTranslationHandle.SM_ScreenTranslationHandle'"));
	if (TMesh.Succeeded())
	{
		Origin->SetStaticMesh(TMesh.Object);
		Origin->SetCollisionProfileName(TEXT("NoCollision"));
	}
	
	
	ConstructorHelpers::FObjectFinder<UMaterial> OriginMaterial(TEXT("/Script/Engine.Material'/Game/JSH/BP/Gizmo/MM_Gizmo_White.MM_Gizmo_White'"));
	if (OriginMaterial.Succeeded())
	{
		Origin->SetMaterial(0, OriginMaterial.Object);
	}


	ConstructorHelpers::FObjectFinder<UMaterial> YellowMaterialLoader(TEXT("/Script/Engine.Material'/Game/JSH/BP/Gizmo/MM_Gizmo_Yellow.MM_Gizmo_Yellow'"));
	if (YellowMaterialLoader.Succeeded())
	{
		YellowMaterial = YellowMaterialLoader.Object;
	}

	ConstructorHelpers::FObjectFinder<UMaterial> BlueMaterialLoader(TEXT("/Script/Engine.Material'/Game/JSH/BP/Gizmo/MM_Gizmo_White.MM_Gizmo_White'"));
	if (BlueMaterialLoader.Succeeded())
	{
		WhiteMaterial = BlueMaterialLoader.Object;
	}

}


// Called when the game starts or when spawned
void AJSH_Translate_GizmoBox::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Error, TEXT("Component 2222"));
	
	JPlayerController = Cast<AJSH_PlayerController>(GetWorld()->GetFirstPlayerController());
	OriginPlayer = Cast<AJSH_Player>(JPlayerController->GetPawn());

	
}

// Called every frame
void AJSH_Translate_GizmoBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
	// if (!OriginPlayer->Gizmo_TranslateMode)
	// {
	// 	if (Origin->GetCollisionProfileName() != TEXT("NoCollision"))
	// 	{
	// 		Origin->SetVisibility(false);
	// 		Origin->SetCollisionProfileName(TEXT("NoCollision"));
	// 	}
	// }
	// else
	// {
	// 	if (Origin->GetCollisionProfileName() != TEXT("BlockAllDynamic"))
	// 	{
	// 		Origin->SetVisibility(true);
	// 		Origin->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	// 	}
	// }
	

	// Editor Mode 마우스 우클릭 시 초기화
	if (OriginPlayer->DisableEdit_b)
	{
		HandleMouseReleaseOutsideActor();
	}

	
	if (Clicked)
	{
		
		NotifyActorOnClicked();
		
		if (JPlayerController->WasInputKeyJustReleased(EKeys::LeftMouseButton)) 
		{
			HandleMouseReleaseOutsideActor();
		}
	}
}


void AJSH_Translate_GizmoBox::NotifyActorOnClicked(FKey ButtonPressed)
{
	if (!CursorOveringGizmo) return;
	
	Super::NotifyActorOnClicked(ButtonPressed);

	// if (OriginPlayer->Editor_SpawnActor->GizmoX_ON || OriginPlayer->Editor_SpawnActor->GizmoZ_ON) return;
	//
	// // Activate Gizmo Y only if it isn't already on
	// if (!OriginPlayer->Editor_SpawnActor->GizmoY_ON)
	// {
	// 	OriginPlayer->Editor_SpawnActor->GizmoY_ON = true;
	// }
	//
	// UE_LOG(LogTemp, Error, TEXT("z1"));
	// if (OriginPlayer != nullptr)
	// {
	// 	FVector GizmoLocation = GetActorLocation();
	// 	FVector PlayerLocation = OriginPlayer->GetActorLocation();
	//
	// 	Lay_Distance = FVector::Dist(GizmoLocation, PlayerLocation);
	// 	Lay_Distance = FMath::Clamp(Lay_Distance, 0.0f, 4000.0f);
	// }
	//
	// // Convert 2D mouse position to 3D world position
	// if (JPlayerController->GetMousePosition(MousePosition.X, MousePosition.Y))
	// {
	// 	JPlayerController->DeprojectMousePositionToWorld(Mouse_WorldLocation, Mouse_WorldDirection);
	// }
	//
	// Start = Mouse_WorldLocation;
	// End = (Mouse_WorldDirection * Lay_Distance) + Mouse_WorldLocation;
	//
	// bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);
	//
	// // Handling for the first click
	// if (bHit && !firstclick && !Clicked)
	// {
	// 	Clicked = true;
	// 	firstclick = true;
	// 	
	// 	UE_LOG(LogTemp, Error, TEXT("z3"));
	// 	
	// 	// Store initial mouse and gizmo positions
	// 	StartMouselocation = HitResult.ImpactPoint;
	// 	StartGizmoLocation = OriginPlayer->Editor_SpawnActor->GizmoActor->GetActorLocation();
	// 	StartActor_Location = StartMouselocation - StartGizmoLocation;
	// 	SelectedGizmo = true;
	// }
	//
	// // Handling for subsequent frames when moving gizmo
	// if (Clicked)
	// {
	//
	// 	if (bHit)
	// 	{
	// 		DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 1, 0, 0.3);
	// 		ConsecutiveHitCount++;
	// 		UE_LOG(LogTemp, Error, TEXT("hit: %d"), ConsecutiveHitCount);
	// 		ConsecutiveMissCount = 0; // 히트가 발생하면 미스 카운트 초기화
	// 	}
	// 	else
	// 	{
	// 		DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 1, 0, 0.3);
	// 		ConsecutiveMissCount++;
	// 		UE_LOG(LogTemp, Error, TEXT("miss: %d"), ConsecutiveMissCount);
	// 		ConsecutiveHitCount = 0; // 미스가 발생하면 히트 카운트 초기화
	// 	}
	//
	// 	// 연속으로 히트가 두 번 발생한 경우에만 히트 처리
	// 	if (ConsecutiveHitCount >= 5)
	// 	{
	// 		
	// 		UE_LOG(LogTemp, Error, TEXT("z4"));
	// 		End_Location = HitResult.ImpactPoint;
	// 		NewLocation = FVector(StartGizmoLocation.X, StartGizmoLocation.Y, End_Location.Z - StartActor_Location.Z);
	// 		OriginPlayer->Editor_SpawnActor->SetActorLocation(NewLocation);
	// 	}
	// 	// 연속으로 미스가 두 번 발생한 경우에만 미스 처리
	// 	else if (ConsecutiveMissCount >= 5)
	// 	{
	// 		
	// 		UE_LOG(LogTemp, Error, TEXT("z5"));
	// 		End_Location = End;
	// 		NewLocation = FVector(StartGizmoLocation.X, StartGizmoLocation.Y, End_Location.Z - StartActor_Location.Z);
	// 		OriginPlayer->Editor_SpawnActor->SetActorLocation(NewLocation);
	// 	}
	//
	// 	
	// 	// // Handle case when hit is detected
	// 	// if (bHit)
	// 	// {
	// 	// 	DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 1, 0, 0.3);
	// 	// 	UE_LOG(LogTemp, Error, TEXT("y4"));
	// 	// 	End_Location = HitResult.ImpactPoint;
	// 	// 	NewLocation = FVector(StartGizmoLocation.X, End_Location.Y - StartActor_Location.Y, StartGizmoLocation.Z);
	// 	// 	OriginPlayer->Editor_SpawnActor->SetActorLocation(NewLocation);
	// 	// }
	// 	// // Handle case when no hit is detected
	// 	// else
	// 	// {
	// 	// 	DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 1, 0, 0.3);
	// 	// 	UE_LOG(LogTemp, Error, TEXT("y5"));
	// 	// 	End_Location = End;
	// 	// 	NewLocation = FVector(StartGizmoLocation.X, End_Location.Y - StartActor_Location.Y, StartGizmoLocation.Z);
	// 	// 	OriginPlayer->Editor_SpawnActor->SetActorLocation(NewLocation);
	// 	// }
	// }
}


// void AJSH_Translate_GizmoBox::NotifyActorOnReleased(FKey ButtonReleased)
// {
// 	Super::NotifyActorOnReleased(ButtonReleased);
//
// 	// Clicked = false;
// 	// SelectedGizmo = false;
// 	OriginColor();
// }


// 오버랩 색상 변경
void AJSH_Translate_GizmoBox::NotifyActorBeginCursorOver()
{
	Super::NotifyActorBeginCursorOver();


	if (OriginPlayer->Editor_SpawnActor->GizmoX_ON) return;
	if (OriginPlayer->Editor_SpawnActor->GizmoY_ON) return;
	
	SelectedColor();
	CursorOveringGizmo = true;
}

void AJSH_Translate_GizmoBox::NotifyActorEndCursorOver()
{
	Super::NotifyActorEndCursorOver();

	if (!Clicked)
	{
		OriginColor();
	}
	// HandleMouseReleaseOutsideActor();
}


void AJSH_Translate_GizmoBox::OriginColor()
{
	if (SelectedGizmo) return;
	
	if (WhiteMaterial)
	{
		Origin->SetMaterial(0, WhiteMaterial);
	}
	// Selected->SetVisibility(false);
	// Origin->SetVisibility(true);
}

void AJSH_Translate_GizmoBox::SelectedColor()
{
	if (YellowMaterial)
	{
		Origin->SetMaterial(0, YellowMaterial);
	}
	// Selected->SetVisibility(true);
	// Origin->SetVisibility(false);
}

void AJSH_Translate_GizmoBox::EndClick()
{
	Clicked = false;
	SelectedGizmo = false;
	OriginColor();
}


void AJSH_Translate_GizmoBox::HandleMouseReleaseOutsideActor()
{
	Clicked = false;
	firstclick = false;
	DuplicateSelected = true;
	SelectedGizmo = false;
	CursorOveringGizmo = false;
	OriginPlayer->Editor_SpawnActor->GizmoZ_ON = false;
	OriginColor();
}