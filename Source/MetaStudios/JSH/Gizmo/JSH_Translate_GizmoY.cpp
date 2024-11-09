// Fill out your copyright notice in the Description page of Project Settings.


#include "../JSH/Gizmo/JSH_Translate_GizmoY.h"
#include "MetaStudios/JSH/JSH_PlayerController.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "Components/PrimitiveComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/EngineTypes.h" 
#include "MetaStudios/JSH/JSH_Editor_SpawnActor.h"


// Sets default values
AJSH_Translate_GizmoY::AJSH_Translate_GizmoY()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 기본 색상
	Origin = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Origin"));
	RootComponent = Origin;
	ConstructorHelpers::FObjectFinder<UStaticMesh> TMesh(TEXT("/Script/Engine.StaticMesh'/Game/JSH/BP/Gizmo/Translate_X/SM_TranslationHandle_X.SM_TranslationHandle_X'"));
	if (TMesh.Succeeded())
	{
		Origin->SetStaticMesh(TMesh.Object);
	}
	
	
	ConstructorHelpers::FObjectFinder<UMaterial> OriginMaterial(TEXT("/Script/Engine.Material'/Game/JSH/BP/Gizmo/MM_Gizmo_Green.MM_Gizmo_Green'"));
	if (OriginMaterial.Succeeded())
	{
		Origin->SetMaterial(0, OriginMaterial.Object);
	}


	ConstructorHelpers::FObjectFinder<UMaterial> YellowMaterialLoader(TEXT("/Script/Engine.Material'/Game/JSH/BP/Gizmo/MM_Gizmo_Yellow.MM_Gizmo_Yellow'"));
	if (YellowMaterialLoader.Succeeded())
	{
		YellowMaterial = YellowMaterialLoader.Object;
	}

	ConstructorHelpers::FObjectFinder<UMaterial> GreenMaterialLoader(TEXT("/Script/Engine.Material'/Game/JSH/BP/Gizmo/MM_Gizmo_Green.MM_Gizmo_Green'"));
	if (GreenMaterialLoader.Succeeded())
	{
		GreenMaterial = GreenMaterialLoader.Object;
	}
}

// Called when the game starts or when spawned
void AJSH_Translate_GizmoY::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Error, TEXT("Component 2222"));
	
	JPlayerController = Cast<AJSH_PlayerController>(GetWorld()->GetFirstPlayerController());
	if (JPlayerController)
	{
		UE_LOG(LogTemp, Error, TEXT("Begin_ScaleX"));
	}

	OriginPlayer = Cast<AJSH_Player>(JPlayerController->GetPawn());

	
}

// Called every frame
void AJSH_Translate_GizmoY::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Clicked)
	{
		NotifyActorOnClicked();


		if (JPlayerController->WasInputKeyJustReleased(EKeys::LeftMouseButton)) 
		{
			HandleMouseReleaseOutsideActor();
		}
	}
}


void AJSH_Translate_GizmoY::NotifyActorOnClicked(FKey ButtonPressed)
{
	if (!CursorOveringGizmo) return;
	
	Super::NotifyActorOnClicked(ButtonPressed);
	
	
	if (OriginPlayer != nullptr)
	{
		// 두 개체의 현재 위치
		FVector GizmoLocation = GetActorLocation();
		FVector PlayerLocation = OriginPlayer->GetActorLocation();

		// 두 개체 사이의 거리 계산
		Lay_Distance = FVector::Dist(GizmoLocation, PlayerLocation);
	}

	
	// 마우스 2D -> 3D Vector 변환
	if (JPlayerController->GetMousePosition(MousePosition.X, MousePosition.Y))
	{
		JPlayerController->DeprojectMousePositionToWorld(Mouse_WorldLocation, Mouse_WorldDirection);
	}
	
	Start = Mouse_WorldLocation;
	End =  (Mouse_WorldDirection * Lay_Distance) + Mouse_WorldLocation;
	
	// FHitResult HitResult;
	// FCollisionQueryParams Params;
	//Params.AddIgnoredActor(OriginPlayer->Editor_SpawnActor);
	
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);
	if (bHit)
	{
		//DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 1, 0, 0.3);
		
		if (!firstclick && Clicked == false)
		{
			if (!Clicked)
			{
				Clicked = true;
			}
			
			firstclick = true;
			// 처음 마우스 위치 저장
			// Start_Mouse_WorldLocation = HitResult.Location.Y;
			
			StartMouselocation = HitResult.ImpactPoint;
			StartGizmoLocation = OriginPlayer->Editor_SpawnActor->GizmoActor->GetActorLocation();
			StartActor_Location = StartMouselocation - StartGizmoLocation;
			//float GapY = StartMouselocation.Y - StartGizmoLocation.Y;
			UE_LOG(LogTemp, Error, TEXT("point %s"), *HitResult.ImpactPoint.ToString());
			UE_LOG(LogTemp, Error, TEXT("gizmo %s"), *StartGizmoLocation.ToString());
			SelectedGizmo = true;
		}
		else
		{
			End_Location = HitResult.ImpactPoint;
			//FVector see = StartMouselocation - End_Location;
			
			NewLocation = FVector(StartGizmoLocation.X, End_Location.Y - StartActor_Location.Y, StartGizmoLocation.Z);
			OriginPlayer->Editor_SpawnActor->SetActorLocation(NewLocation);
			
			firstclick = false;
		}
	}
	else
	{
		//DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 1, 0, 0.3);
		End_Location = End;
		NewLocation = FVector(StartGizmoLocation.X, End_Location.Y - StartActor_Location.Y, StartGizmoLocation.Z);
		OriginPlayer->Editor_SpawnActor->SetActorLocation(NewLocation);
	}
}



void AJSH_Translate_GizmoY::NotifyActorOnReleased(FKey ButtonReleased)
{
	Super::NotifyActorOnReleased(ButtonReleased);

	// Clicked = false;
	// SelectedGizmo = false;
	OriginColor();
}


// 오버랩 색상 변경
void AJSH_Translate_GizmoY::NotifyActorBeginCursorOver()
{
	Super::NotifyActorBeginCursorOver();

	SelectedColor();
	CursorOveringGizmo = true;
}
void AJSH_Translate_GizmoY::NotifyActorEndCursorOver()
{
	Super::NotifyActorEndCursorOver();

	if (!Clicked)
	{
		OriginColor();
	}
	// HandleMouseReleaseOutsideActor();
}


void AJSH_Translate_GizmoY::OriginColor()
{
	if (SelectedGizmo) return;
	
	if (GreenMaterial)
	{
		Origin->SetMaterial(0, GreenMaterial);
	}
	// Selected->SetVisibility(false);
	// Origin->SetVisibility(true);
}

void AJSH_Translate_GizmoY::SelectedColor()
{
	if (YellowMaterial)
	{
		Origin->SetMaterial(0, YellowMaterial);
	}
	// Selected->SetVisibility(true);
	// Origin->SetVisibility(false);
}

void AJSH_Translate_GizmoY::EndClick()
{
	Clicked = false;
	SelectedGizmo = false;
	OriginColor();
}


void AJSH_Translate_GizmoY::HandleMouseReleaseOutsideActor()
{
	Clicked = false;
	SelectedGizmo = false;
	CursorOveringGizmo = false;
	OriginColor();
}