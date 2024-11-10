// Fill out your copyright notice in the Description page of Project Settings.


#include "../JSH/Gizmo/JSH_Translate_GizmoY.h"
#include "MetaStudios/JSH/JSH_PlayerController.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "Components/PrimitiveComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/EngineTypes.h"
#include "JSH_Translate_GizmoZ.h"
#include "JSH_Translate_GizmoX.h"
#include "JSH_Translate_GizmoBox.h"
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
	
	JPlayerController = Cast<AJSH_PlayerController>(GetWorld()->GetFirstPlayerController());
	OriginPlayer = Cast<AJSH_Player>(JPlayerController->GetPawn());
	if (OriginPlayer)
	{
		OriginPlayer->Save_Gizmo_TY(this);
	}

	//FindAndStoreGizmoActors(GetWorld());
}

// Called every frame
void AJSH_Translate_GizmoY::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
	// Editor Mode 마우스 우클릭 시 초기화
	if (OriginPlayer->DisableEdit_b)
	{
		HandleMouseReleaseOutsideActor();
	}

	// Gizmo 클릭 시 Tick으로 NotifyActorOnClicked() 돌리기 위한 (Actor에는 Trigger처럼 못함)
	if (Clicked)
	{
		NotifyActorOnClicked();

		// 마우스 왼쪽 클릭을 놓았을 떄에
		if (JPlayerController->WasInputKeyJustReleased(EKeys::LeftMouseButton)) 
		{
			HandleMouseReleaseOutsideActor();
		}
	}
}





void AJSH_Translate_GizmoY::NotifyActorOnClicked(FKey ButtonPressed)
{
	Super::NotifyActorOnClicked(ButtonPressed);

	// Cursor에 오버랩 되었을때 True로 바뀌는 bool값임 , 커서에 마우스 올라가 있을때에만 클릭해도 실행되도록 (왜 넣었는지 기억 안남, 없어도 될듯 싶음)
	if (!CursorOveringGizmo) return;
	
	//// 다른 기즈모가 실행 중 이면 , 기능 실행되지 않도록 ////
	if (OriginPlayer->Editor_SpawnActor->GizmoX_ON || OriginPlayer->Editor_SpawnActor->GizmoZ_ON || OriginPlayer->Editor_SpawnActor->GizmoB_ON) return;
	if (!OriginPlayer->Editor_SpawnActor->GizmoY_ON)
	{
		OriginPlayer->Editor_SpawnActor->GizmoY_ON = true;
	}

	// 중복을 막기 위해 사전에 생성자에서 NoCollision 해줬던거를 변경
	//Origin->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	
	UE_LOG(LogTemp, Error, TEXT("y1"));

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


	TArray<AActor*> IgnoreGizmos;
	IgnoreGizmos.Add(OriginPlayer->Saved_Gizmo_TX);
	IgnoreGizmos.Add(OriginPlayer->Saved_Gizmo_TZ);
	IgnoreGizmos.Add(OriginPlayer->Saved_Gizmo_TB);
	Params.AddIgnoredActors(IgnoreGizmos);
	
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);

	
	
	///// 처음 클릭했을때 값 저장하기 위한 함수 ////
	if (bHit && !firstclick && !Clicked)
	{
		Clicked = true;
		firstclick = true;
		
		UE_LOG(LogTemp, Error, TEXT("y first"));
		
		// Store initial mouse and gizmo positions
		StartMouselocation = HitResult.ImpactPoint;
		StartGizmoLocation = OriginPlayer->Editor_SpawnActor->GizmoActor->GetActorLocation();
		StartActor_Location = StartMouselocation - StartGizmoLocation;
		SelectedGizmo = true;
	}

	
	///// 처음 클릭 되고 난 후 돌아가는 함수 ////
	if (Clicked)
	{
		// bHit 되었을 떄엔 Impact Point를 통해서 위치 이동
		if (bHit)
		{
			//DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 1, 0, 0.3);
			UE_LOG(LogTemp, Error, TEXT("y2"));
			End_Location = HitResult.ImpactPoint;
			NewLocation = FVector(StartGizmoLocation.X, End_Location.Y - StartActor_Location.Y, StartGizmoLocation.Z);
			OriginPlayer->Editor_SpawnActor->SetActorLocation(NewLocation);
		}
		// bHit 되지 않았을 떄엔 Ray 끝점을 통해서 위치 이동 (위에서 구한 Player와 Gizmo 사이의 거리)
		else
		{
			//DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 1, 0, 0.3);
			UE_LOG(LogTemp, Error, TEXT("y3"));
			End_Location = End;
			NewLocation = FVector(StartGizmoLocation.X, End_Location.Y - StartActor_Location.Y, StartGizmoLocation.Z);
			OriginPlayer->Editor_SpawnActor->SetActorLocation(NewLocation);
		}
	}
}

// 오버랩 색상 변경
void AJSH_Translate_GizmoY::NotifyActorBeginCursorOver()
{
	Super::NotifyActorBeginCursorOver();


	if (OriginPlayer->Editor_SpawnActor->GizmoX_ON) return;
	if (OriginPlayer->Editor_SpawnActor->GizmoZ_ON) return;
	if (OriginPlayer->Editor_SpawnActor->GizmoB_ON) return;
	
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
	// Gizmo가 클릭된 상태라면 , 마우스가 Gizmo 위에 있지 않아도 계속해서 노란색 유지하기 위해
	if (SelectedGizmo) return;
	
	if (GreenMaterial)
	{
		Origin->SetMaterial(0, GreenMaterial);
	}
}

void AJSH_Translate_GizmoY::SelectedColor()
{
	if (YellowMaterial)
	{
		Origin->SetMaterial(0, YellowMaterial);
	}
}


void AJSH_Translate_GizmoY::HandleMouseReleaseOutsideActor()
{
	Clicked = false;
	firstclick = false;
	SelectedGizmo = false;
	CursorOveringGizmo = false;
	OriginPlayer->Editor_SpawnActor->GizmoY_ON = false;
	OriginColor();

	// Else문 반복 실행을 막기 위해
	// Origin->SetCollisionProfileName(TEXT("NoCollision"));
}


//// Player쪽에서 Gizmo Mode 바꿀때 조정해줌  ////
void AJSH_Translate_GizmoY::Visible_and_Collision_On()
{
	Origin->SetVisibility(true);
	Origin->SetCollisionProfileName(TEXT("BlockAllDynamic"));
}
void AJSH_Translate_GizmoY::Visible_and_Collision_Off()
{
	Origin->SetVisibility(false);
	Origin->SetCollisionProfileName(TEXT("NoCollision"));
}