// Fill out your copyright notice in the Description page of Project Settings.



#include "../Gizmo/JSH_Scale_GizmoZ.h"
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
#include "JSH_Scale_GizmoX.h"
#include "JSH_Rotate_GizmoX.h"
#include "JSH_Rotate_GizmoY.h"
#include "JSH_Rotate_GizmoZ.h"
#include "Engine/EngineTypes.h"
#include "MetaStudios/JSH/JSH_Editor_SpawnActor.h"


// Sets default values
AJSH_Scale_GizmoZ::AJSH_Scale_GizmoZ()
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
		Origin->SetCollisionProfileName(TEXT("Gizmo"));
		Origin->SetVisibility(false);
		Origin->SetCollisionProfileName(TEXT("NoCollision"));
	}
	
	
	ConstructorHelpers::FObjectFinder<UMaterial> OriginMaterial(TEXT("/Script/Engine.Material'/Game/JSH/BP/Gizmo/MM_Gizmo_Blue.MM_Gizmo_Blue'"));
	if (OriginMaterial.Succeeded())
	{
		Origin->SetMaterial(0, OriginMaterial.Object);
	}


	ConstructorHelpers::FObjectFinder<UMaterial> YellowMaterialLoader(TEXT("/Script/Engine.Material'/Game/JSH/BP/Gizmo/MM_Gizmo_Yellow.MM_Gizmo_Yellow'"));
	if (YellowMaterialLoader.Succeeded())
	{
		BlueMaterial = YellowMaterialLoader.Object;
	}

	ConstructorHelpers::FObjectFinder<UMaterial> RedMaterialLoader(TEXT("/Script/Engine.Material'/Game/JSH/BP/Gizmo/MM_Gizmo_Blue.MM_Gizmo_Blue'"));
	if (RedMaterialLoader.Succeeded())
	{
		RedMaterial = RedMaterialLoader.Object;
	}
	
	Tags.Add(FName("Scale_Gizmo_Z"));
}


// Called when the game starts or when spawned
void AJSH_Scale_GizmoZ::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AJSH_Scale_GizmoZ::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
	// Editor Mode 마우스 우클릭 시 초기화
	if (OriginPlayer != nullptr)
	{
		if (OriginPlayer->DisableEdit_b)
		{
			HandleMouseReleaseOutsideActor();
		}
	}
	
	if (Clicked)
	{
		GOnClicked();
	}
	
}




void AJSH_Scale_GizmoZ::NotifyActorOnClicked(FKey ButtonPressed)
{
	Super::NotifyActorOnClicked(ButtonPressed);

}

void AJSH_Scale_GizmoZ::GOnClicked()
{
	// Cursor에 오버랩 되었을때 True로 바뀌는 bool값임 , 커서에 마우스 올라가 있을때에만 클릭해도 실행되도록 (왜 넣었는지 기억 안남, 없어도 될듯 싶음)
	if (!CursorOveringGizmo) return;
	
	
	//// 다른 기즈모가 실행 중 이면 , 기능 실행되지 않도록 ////
	if (OriginPlayer->Editor_SpawnActor->GizmoY_ON || OriginPlayer->Editor_SpawnActor->GizmoX_ON || OriginPlayer->Editor_SpawnActor->GizmoB_ON) return;
	if (!OriginPlayer->Editor_SpawnActor->GizmoZ_ON)
	{
		OriginPlayer->Editor_SpawnActor->GizmoZ_ON = true;
	}
	

	//// Gizmo와 Player간의 거리 구하기 (bHit되지 않았을때 최대 거리 point로 hitpoint 잡아야함) ////
	if (OriginPlayer != nullptr)
	{
		FVector GizmoLocation = GetActorLocation();
		FVector PlayerLocation = OriginPlayer->GetActorLocation();
	
		Lay_Distance = FVector::Dist(GizmoLocation, PlayerLocation);
		// 거리를 너무 늘리면, 꾹 누르고 있을때 , 너무 멀리 나아가 버림
		Lay_Distance = FMath::Clamp(Lay_Distance, 0.0f, 4000.0f); 
	}
	
	//// 마우스 2d Vector -> 3d Vector ////
	if (JPlayerController->GetMousePosition(MousePosition.X, MousePosition.Y))
	{
		JPlayerController->DeprojectMousePositionToWorld(Mouse_WorldLocation, Mouse_WorldDirection);
	}

	///// Ray ////
	Start = Mouse_WorldLocation;
	End = (Mouse_WorldDirection * Lay_Distance) + Mouse_WorldLocation;

	// 다 만들고 추가
	TArray<AActor*> IgnoreGizmos;
	IgnoreGizmos.Add(OriginPlayer->Saved_Gizmo_TX);
	IgnoreGizmos.Add(OriginPlayer->Saved_Gizmo_TY);
	IgnoreGizmos.Add(OriginPlayer->Saved_Gizmo_TZ);
	IgnoreGizmos.Add(OriginPlayer->Saved_Gizmo_TB);
	
	IgnoreGizmos.Add(OriginPlayer->Saved_Gizmo_SY);
	IgnoreGizmos.Add(OriginPlayer->Saved_Gizmo_SX);
	IgnoreGizmos.Add(OriginPlayer->Saved_Gizmo_SB);

	IgnoreGizmos.Add(OriginPlayer->Saved_Gizmo_RX);
	IgnoreGizmos.Add(OriginPlayer->Saved_Gizmo_RY);
	IgnoreGizmos.Add(OriginPlayer->Saved_Gizmo_RZ);
	Params.AddIgnoredActors(IgnoreGizmos);
	
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_GameTraceChannel1, Params);
	///// 처음 클릭했을때 값 저장하기 위한 함수 ////
	if (bHit && !firstclick && !Clicked)
	{
		Clicked = true;
		firstclick = true;
		
		// Store initial mouse and gizmo positions
		//StartMouselocation = HitResult.ImpactPoint;
		StartMouselocation = End;
		Start_Scale = OriginPlayer->Editor_SpawnActor->GetActorRelativeScale3D();
		
		SelectedGizmo = true;
	}
	else if (!firstclick && !Clicked)
	{
		Clicked = true;
		firstclick = true;
		
		// Store initial mouse and gizmo positions
		StartMouselocation = End;
		Start_Scale = OriginPlayer->Editor_SpawnActor->GetActorRelativeScale3D();
		SelectedGizmo = true;
	}

	
	
	///// 처음 클릭 되고 난 후 돌아가는 함수 ////
	if (Clicked)
	{
		End_Location = End;

		End_Scale = FVector(Start_Scale.X, Start_Scale.Y, Start_Scale.Z + (( End_Location.Z - StartMouselocation.Z) * 0.01));

		// -값으로 가면 3d object 앞뒤가 뒤집혀 버어림
		//End_Scale = FVector(FMath::Abs(Start_Scale.X), FMath::Abs(Start_Scale.Y), FMath::Abs(Start_Scale.Z + ((End_Location.Z - StartMouselocation.Z) * 0.01)));
		//OriginPlayer->Editor_SpawnActor->SetActorRelativeScale3D(End_Scale);
		OriginPlayer->Editor_SpawnActor->Set_Scale_from_Gizmo(End_Scale);
	}


	// ///// 처음 클릭 되고 난 후 돌아가는 함수 ////
	// if (Clicked)
	// {
	// 	End_Location = End;
	// 	//End_Scale = FVector(Start_Scale.X + (( End_Location.X - StartMouselocation.X) * 0.01), Start_Scale.Y, Start_Scale.Z);
	//
	// 	// -값으로 가면 3d object 앞뒤가 뒤집혀 버어림
	// 	End_Scale = FVector(FMath::Abs(Start_Scale.X), FMath::Abs(Start_Scale.Y), FMath::Abs(Start_Scale.Z + ((End_Location.Z - StartMouselocation.Z) * 0.01)));
	// 	//OriginPlayer->Editor_SpawnActor->SetActorRelativeScale3D(End_Scale);
	// 	OriginPlayer->Editor_SpawnActor->Set_Scale_from_Gizmo(End_Scale);
	// }
}

// 오버랩 색상 변경
void AJSH_Scale_GizmoZ::BeginCursorOver()
{
	if (OriginPlayer->Editor_SpawnActor->GizmoY_ON) return;
	if (OriginPlayer->Editor_SpawnActor->GizmoX_ON) return;
	if (OriginPlayer->Editor_SpawnActor->GizmoB_ON) return;
	
	SelectedColor();
	CursorOveringGizmo = true;
}

void AJSH_Scale_GizmoZ::EndCursorOver()
{
	////Super::NotifyActorEndCursorOver();

	if (!Clicked)
	{
		OriginColor();
	}
	// HandleMouseReleaseOutsideActor();
}


void AJSH_Scale_GizmoZ::OriginColor()
{
	// Gizmo가 클릭된 상태라면 , 마우스가 Gizmo 위에 있지 않아도 계속해서 노란색 유지하기 위해
	if (SelectedGizmo) return;
	
	if (RedMaterial)
	{
		Origin->SetMaterial(0, RedMaterial);
	}
}

void AJSH_Scale_GizmoZ::SelectedColor()
{
	if (BlueMaterial)
	{
		Origin->SetMaterial(0, BlueMaterial);
	}
}


void AJSH_Scale_GizmoZ::HandleMouseReleaseOutsideActor()
{
	Clicked = false;
	firstclick = false;
	SelectedGizmo = false;
	CursorOveringGizmo = false;
	if (OriginPlayer->Editor_SpawnActor != nullptr && OriginPlayer->Editor_SpawnActor->GizmoZ_ON != false)
	{
		OriginPlayer->Editor_SpawnActor->GizmoZ_ON = false;
	}
	OriginColor();

	// Else문 반복 실행을 막기 위해
	// Origin->SetCollisionProfileName(TEXT("NoCollision"));
}


//// Player쪽에서 Gizmo Mode 바꿀때 조정해줌  ////
void AJSH_Scale_GizmoZ::Visible_and_Collision_On()
{
	Origin->SetVisibility(true);
	Origin->SetCollisionProfileName(TEXT("Gizmo"));
}
void AJSH_Scale_GizmoZ::Visible_and_Collision_Off()
{
	Origin->SetVisibility(false);
	Origin->SetCollisionProfileName(TEXT("NoCollision"));
}

void AJSH_Scale_GizmoZ::BeginPlayer(AJSH_Player* temp, AJSH_PlayerController* control)
{
	OriginPlayer = temp;
	if (OriginPlayer)
	{
		OriginPlayer->Save_Gizmo_SZ(this);
	}

	JPlayerController = control;
}
