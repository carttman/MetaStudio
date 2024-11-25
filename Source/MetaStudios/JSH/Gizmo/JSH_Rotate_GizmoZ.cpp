// Fill out your copyright notice in the Description page of Project Settings.


#include "JSH_Rotate_GizmoZ.h"
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
#include "JSH_Rotate_GizmoY.h"
#include "JSH_Rotate_GizmoX.h"
#include "Engine/EngineTypes.h" 
#include "MetaStudios/JSH/JSH_Editor_SpawnActor.h"


// Sets default values
AJSH_Rotate_GizmoZ::AJSH_Rotate_GizmoZ()
// Fill out your copyright notice in the Description page of Project Settings.
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;




	Root_Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Root_Scene"));
	RootComponent = Root_Scene;
	
	
	Origin = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Origin"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> TMesh(TEXT("/Script/Engine.StaticMesh'/Game/JSH/Material/Rotate/circle.circle'"));
	if (TMesh.Succeeded())
	{
		Origin->SetupAttachment(Root_Scene);
		Origin->SetStaticMesh(TMesh.Object);
		Origin->SetCollisionProfileName(TEXT("Gizmo"));
		Origin->SetVisibility(false);
		Origin->SetCollisionProfileName(TEXT("NoCollision"));
	}
	
	Origin2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Origin2"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> TMesh2(TEXT("/Script/Engine.StaticMesh'/Game/JSH/Material/Rotate/circle.circle'"));
	if (TMesh2.Succeeded())
	{
		Origin2->SetupAttachment(Root_Scene);
		Origin2->SetStaticMesh(TMesh2.Object);
		Origin2->SetCollisionProfileName(TEXT("Gizmo"));
		Origin2->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));
		Origin2->SetVisibility(false);
		Origin2->SetCollisionProfileName(TEXT("NoCollision"));
	}
	Origin3 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Origin3"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> TMesh3(TEXT("/Script/Engine.StaticMesh'/Game/JSH/Material/Rotate/circle.circle'"));
	if (TMesh3.Succeeded())
	{
		Origin3->SetupAttachment(Root_Scene);
		Origin3->SetStaticMesh(TMesh3.Object);
		Origin3->SetCollisionProfileName(TEXT("Gizmo"));
		Origin3->SetRelativeRotation(FRotator(0.f, 180.f, 0.f));
		Origin3->SetVisibility(false);
		Origin3->SetCollisionProfileName(TEXT("NoCollision"));
	}

	Origin4 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Origin4"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> TMesh4(TEXT("/Script/Engine.StaticMesh'/Game/JSH/Material/Rotate/circle.circle'"));
	if (TMesh4.Succeeded())
	{
		Origin4->SetupAttachment(Root_Scene);
		Origin4->SetStaticMesh(TMesh4.Object);
		Origin4->SetCollisionProfileName(TEXT("Gizmo"));
		Origin4->SetRelativeRotation(FRotator(0.f, 270.f, 0.f));
		Origin4->SetVisibility(false);
		Origin4->SetCollisionProfileName(TEXT("NoCollision"));
	}

	ConstructorHelpers::FObjectFinder<UMaterial> OriginMaterial(TEXT("/Script/Engine.Material'/Game/JSH/BP/Gizmo/MM_Rotate_Blue.MM_Rotate_Blue'"));
	if (OriginMaterial.Succeeded())
	{
		Origin->SetMaterial(0, OriginMaterial.Object);
		Origin2->SetMaterial(0, OriginMaterial.Object);
		Origin3->SetMaterial(0, OriginMaterial.Object);
		Origin4->SetMaterial(0, OriginMaterial.Object);
	}


	ConstructorHelpers::FObjectFinder<UMaterial> YellowMaterialLoader(TEXT("/Script/Engine.Material'/Game/JSH/BP/Gizmo/MM_Rotate_Blue_Yellow.MM_Rotate_Blue_Yellow'"));
	if (YellowMaterialLoader.Succeeded())
	{
		YellowMaterial = YellowMaterialLoader.Object;
	}

	ConstructorHelpers::FObjectFinder<UMaterial> RedMaterialLoader(TEXT("/Script/Engine.Material'/Game/JSH/BP/Gizmo/MM_Rotate_Blue.MM_Rotate_Blue'"));
	if (RedMaterialLoader.Succeeded())
	{
		BlueMaterial = RedMaterialLoader.Object;
	}
	
	Tags.Add(FName("Rotate_Gizmo_Z"));
}


// Called when the game starts or when spawned
void AJSH_Rotate_GizmoZ::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AJSH_Rotate_GizmoZ::Tick(float DeltaTime)
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




void AJSH_Rotate_GizmoZ::NotifyActorOnClicked(FKey ButtonPressed)
{
	Super::NotifyActorOnClicked(ButtonPressed);

}

void AJSH_Rotate_GizmoZ::GOnClicked()
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

		JPlayerController->GetMousePosition(MouseX, MouseY);
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
	IgnoreGizmos.Add(OriginPlayer->Saved_Gizmo_SZ);
	IgnoreGizmos.Add(OriginPlayer->Saved_Gizmo_SX);
	IgnoreGizmos.Add(OriginPlayer->Saved_Gizmo_SB);

	IgnoreGizmos.Add(OriginPlayer->Saved_Gizmo_RY);
	IgnoreGizmos.Add(OriginPlayer->Saved_Gizmo_RX);
	Params.AddIgnoredActors(IgnoreGizmos);


	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_GameTraceChannel1, Params);
	///// 처음 클릭했을때 값 저장하기 위한 함수 ////
	if (bHit && !firstclick && !Clicked)
	{
		Clicked = true;
		firstclick = true;
		
		UE_LOG(LogTemp, Error, TEXT("y first"));
		
		// Store initial mouse and gizmo positions
		//StartMouselocation = HitResult.ImpactPoint;
		StartMouselocation = End;
		
		StartMouselocation_2D_X = MouseX;
		StartMouselocation_2D_Y = MouseY;
		
		Start_Rotate = OriginPlayer->Editor_SpawnActor->GetActorRotation();
		
		SelectedGizmo = true;
	}
	else if (!firstclick && !Clicked)
	{
		Clicked = true;
		firstclick = true;
		
		// Store initial mouse and gizmo positions
		StartMouselocation = End;
		
		StartMouselocation_2D_X = MouseX;
		StartMouselocation_2D_Y = MouseY;
		
		Start_Rotate = OriginPlayer->Editor_SpawnActor->GetActorRotation();
		SelectedGizmo = true;
	}




	// Editor Actor 중심의 회전이 아닌 , gizmo 중심의 회전을 중심으로 돌리기
	if (Clicked)
	{
		End_Location = End;
		EndMouselocation_2D_X = MouseX;
		EndMouselocation_2D_Y = MouseY;

		//End_variation = ((EndMouselocation_2D_Y - StartMouselocation_2D_Y) * -0.2f) + ((EndMouselocation_2D_X - StartMouselocation_2D_X) * 0.2f);
		End_variation = ((EndMouselocation_2D_X - StartMouselocation_2D_X) * -0.2f);

		// 현재의 Yaw 값을 사용하여 새로운 DeltaRotation 계산
		FRotator DeltaRotation = FRotator(0.0F, End_variation, 0.0f);

		// 현재 회전을 기준으로 DeltaRotation 적용
		FQuat CurrentQuat = Start_Rotate.Quaternion();
		FQuat DeltaQuat = DeltaRotation.Quaternion();
		FQuat NewQuat = DeltaQuat * CurrentQuat;

		// 새로운 Rotation 설정
		End_Rotate = NewQuat.Rotator();

		// 최종 회전 적용
		OriginPlayer->Editor_SpawnActor->Set_Rotate_from_Gizmo(End_Rotate);
	}




	//
	// ///// 처음 클릭 되고 난 후 돌아가는 함수 ////
	// if (Clicked)
	// {
	// 	End_Location = End;
	//
	// 	EndMouselocation_2D_X = MouseX;
	// 	EndMouselocation_2D_Y = MouseY;
	//
	// 	End_variation = ((EndMouselocation_2D_Y - StartMouselocation_2D_Y) * - 0.2) + ((EndMouselocation_2D_X - StartMouselocation_2D_X) * 0.2);
	// 	//End_variation = ((EndMouselocation_2D_X - StartMouselocation_2D_X) * -0.3);
	//
	// 	End_Rotate = FRotator(Start_Rotate.Pitch, Start_Rotate.Yaw  + End_variation, Start_Rotate.Roll);
	//
	// 	OriginPlayer->Editor_SpawnActor->Set_Rotate_from_Gizmo(End_Rotate);
	// }
}




// 오버랩 색상 변경
void AJSH_Rotate_GizmoZ::BeginCursorOver()
{
	if (OriginPlayer->Editor_SpawnActor->GizmoY_ON) return;
	if (OriginPlayer->Editor_SpawnActor->GizmoX_ON) return;
	if (OriginPlayer->Editor_SpawnActor->GizmoB_ON) return;
	
	SelectedColor();
	CursorOveringGizmo = true;
}

void AJSH_Rotate_GizmoZ::EndCursorOver()
{
	////Super::NotifyActorEndCursorOver();

	if (!Clicked)
	{
		OriginColor();
	}
	// HandleMouseReleaseOutsideActor();
}


void AJSH_Rotate_GizmoZ::OriginColor()
{
	// Gizmo가 클릭된 상태라면 , 마우스가 Gizmo 위에 있지 않아도 계속해서 노란색 유지하기 위해
	if (SelectedGizmo) return;
	
	if (BlueMaterial)
	{
		Origin->SetMaterial(0, BlueMaterial);
		Origin2->SetMaterial(0, BlueMaterial);
		Origin3->SetMaterial(0, BlueMaterial);
		Origin4->SetMaterial(0, BlueMaterial);
	}
}

void AJSH_Rotate_GizmoZ::SelectedColor()
{
	if (YellowMaterial)
	{
		Origin->SetMaterial(0, YellowMaterial);
		Origin2->SetMaterial(0, YellowMaterial);
		Origin3->SetMaterial(0, YellowMaterial);
		Origin4->SetMaterial(0, YellowMaterial);
	}
}


void AJSH_Rotate_GizmoZ::HandleMouseReleaseOutsideActor()
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
void AJSH_Rotate_GizmoZ::Visible_and_Collision_On()
{
	Origin->SetVisibility(true);
	Origin->SetCollisionProfileName(TEXT("Gizmo"));
	Origin2->SetVisibility(true);
	Origin2->SetCollisionProfileName(TEXT("Gizmo"));
	Origin3->SetVisibility(true);
	Origin3->SetCollisionProfileName(TEXT("Gizmo"));
	Origin4->SetVisibility(true);
	Origin4->SetCollisionProfileName(TEXT("Gizmo"));
	
}
void AJSH_Rotate_GizmoZ::Visible_and_Collision_Off()
{
	Origin->SetVisibility(false);
	Origin->SetCollisionProfileName(TEXT("NoCollision"));
	Origin2->SetVisibility(false);
	Origin2->SetCollisionProfileName(TEXT("NoCollision"));
	Origin3->SetVisibility(false);
	Origin3->SetCollisionProfileName(TEXT("NoCollision"));
	Origin4->SetVisibility(false);
	Origin4->SetCollisionProfileName(TEXT("NoCollision"));
}

void AJSH_Rotate_GizmoZ::BeginPlayer(AJSH_Player* temp, AJSH_PlayerController* control)
{
	OriginPlayer = temp;
	if (OriginPlayer)
	{
		OriginPlayer->Save_Gizmo_RZ(this);
	}

	JPlayerController = control;
}