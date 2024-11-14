// Fill out your copyright notice in the Description page of Project Settings.


#include "../Gizmo/JSH_Scale_GizmoY.h"
#include "MetaStudios/JSH/JSH_PlayerController.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "Components/PrimitiveComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/EngineTypes.h" 
#include "MetaStudios/JSH/JSH_Editor_SpawnActor.h"

// Sets default values
AJSH_Scale_GizmoY::AJSH_Scale_GizmoY()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 기본 색상
	Origin = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Origin"));
	RootComponent = Origin;
	ConstructorHelpers::FObjectFinder<UStaticMesh> TMesh(TEXT("/Script/Engine.StaticMesh'/Game/JSH/BP/Gizmo/StaticMesh/SM_ScaleHandle.SM_ScaleHandle'"));
	if (TMesh.Succeeded())
	{
		Origin->SetStaticMesh(TMesh.Object);
		Origin->SetVisibility(false);
		Origin->SetCollisionProfileName(TEXT("NoCollision"));
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
void AJSH_Scale_GizmoY::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Error, TEXT("Component 2222"));
	
	JPlayerController = Cast<AJSH_PlayerController>(GetWorld()->GetFirstPlayerController());
	if (JPlayerController)
	{
		UE_LOG(LogTemp, Error, TEXT("Begin_ScaleX"));
	}

	OriginPlayer = Cast<AJSH_Player>(JPlayerController->GetPawn());
	// if (OriginPlayer)
	// {
	// 	OriginPlayer->Save_Gizmo_TY(this);
	// }
	
}

// Called every frame
void AJSH_Scale_GizmoY::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
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


void AJSH_Scale_GizmoY::NotifyActorOnClicked(FKey ButtonPressed)
{
	if (!CursorOveringGizmo) return;
	
	Super::NotifyActorOnClicked(ButtonPressed);
	

	if (OriginPlayer->Editor_SpawnActor->GizmoX_ON) return;
	if (OriginPlayer->Editor_SpawnActor->GizmoZ_ON) return;
	if (!OriginPlayer->Editor_SpawnActor->GizmoY_ON)
	{
		OriginPlayer->Editor_SpawnActor->GizmoY_ON = true;
	}
	
	UE_LOG(LogTemp, Error, TEXT("y1"));
	if (OriginPlayer != nullptr)
	{
		// 두 개체의 현재 위치
		FVector GizmoLocation = GetActorLocation();
		FVector PlayerLocation = OriginPlayer->GetActorLocation();

		// 두 개체 사이의 거리 계산
		Lay_Distance = FVector::Dist(GizmoLocation, PlayerLocation);
		if (Lay_Distance >= 4000.0f)
		{
			Lay_Distance = 4000.0f;
		}
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
	// AActor* dd = Cast<AActor>(OriginPlayer->Saved_Gizmo_TX);
	
	
	
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);
	if (bHit)
	{
		// 다른 축과 겹쳐졌을때 else랑 같이 들어오는 오류가 있씀 
		//if (HitResult.GetActor() != this) return;
	
		//DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 1, 0, 0.3);
		UE_LOG(LogTemp, Error, TEXT("y2"));
		if (!firstclick && !Clicked)
		{
			Clicked = true;
			firstclick = true;
			
			UE_LOG(LogTemp, Error, TEXT("y3"));
			
			// 처음 마우스 위치 저장
			// Start_Mouse_WorldLocation = HitResult.Location.Y;
			
			StartMouselocation = HitResult.ImpactPoint;
			StartGizmoLocation = OriginPlayer->Editor_SpawnActor->GizmoActor->GetActorLocation();
			StartActor_Location = StartMouselocation - StartGizmoLocation;
			//float GapY = StartMouselocation.Y - StartGizmoLocation.Y;
			//UE_LOG(LogTemp, Error, TEXT("point %s"), *HitResult.ImpactPoint.ToString());
			//UE_LOG(LogTemp, Error, TEXT("gizmo %s"), *StartGizmoLocation.ToString());
			SelectedGizmo = true;
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("y4"));
			End_Location = HitResult.ImpactPoint;
			//FVector see = StartMouselocation - End_Location;
			
			NewLocation = FVector(StartGizmoLocation.X, End_Location.Y - StartActor_Location.Y, StartGizmoLocation.Z);
			OriginPlayer->Editor_SpawnActor->SetActorLocation(NewLocation);
			
			//firstclick = false;
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("y5"));
		End_Location = End;
		
		NewLocation = FVector(StartGizmoLocation.X, End_Location.Y - StartActor_Location.Y, StartGizmoLocation.Z);
		OriginPlayer->Editor_SpawnActor->SetActorLocation(NewLocation);
		//
		// firstclick = false;
	}
}



// void AJSH_Scale_GizmoY::NotifyActorOnReleased(FKey ButtonReleased)
// {
// 	Super::NotifyActorOnReleased(ButtonReleased);
//
// 	// Clicked = false;
// 	// SelectedGizmo = false;
// 	OriginColor();
// }


// 오버랩 색상 변경
void AJSH_Scale_GizmoY::NotifyActorBeginCursorOver()
{
	Super::NotifyActorBeginCursorOver();

	if (OriginPlayer->Editor_SpawnActor->GizmoX_ON) return;
	if (OriginPlayer->Editor_SpawnActor->GizmoZ_ON) return;
	
	SelectedColor();
	CursorOveringGizmo = true;
}

void AJSH_Scale_GizmoY::NotifyActorEndCursorOver()
{
	Super::NotifyActorEndCursorOver();

	if (!Clicked)
	{
		OriginColor();
	}
	// HandleMouseReleaseOutsideActor();
}


void AJSH_Scale_GizmoY::OriginColor()
{
	if (SelectedGizmo) return;
	
	if (GreenMaterial)
	{
		Origin->SetMaterial(0, GreenMaterial);
	}
	// Selected->SetVisibility(false);
	// Origin->SetVisibility(true);
}

void AJSH_Scale_GizmoY::SelectedColor()
{
	if (YellowMaterial)
	{
		Origin->SetMaterial(0, YellowMaterial);
	}
	// Selected->SetVisibility(true);
	// Origin->SetVisibility(false);
}


void AJSH_Scale_GizmoY::HandleMouseReleaseOutsideActor()
{
	Clicked = false;
	firstclick = false;
	SelectedGizmo = false;
	CursorOveringGizmo = false;
	OriginPlayer->Editor_SpawnActor->GizmoY_ON = false;
	OriginColor();
}
