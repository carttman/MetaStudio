// Fill out your copyright notice in the Description page of Project Settings.


#include "../JSH/Gizmo/JSH_Translate_GizmoZ.h"
#include "MetaStudios/JSH/JSH_PlayerController.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "Components/PrimitiveComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/EngineTypes.h" 
#include "MetaStudios/JSH/JSH_Editor_SpawnActor.h"


// Sets default values
AJSH_Translate_GizmoZ::AJSH_Translate_GizmoZ()
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
	
	
	ConstructorHelpers::FObjectFinder<UMaterial> OriginMaterial(TEXT("/Script/Engine.Material'/Game/JSH/BP/Gizmo/MM_Gizmo_Blue.MM_Gizmo_Blue'"));
	if (OriginMaterial.Succeeded())
	{
		Origin->SetMaterial(0, OriginMaterial.Object);
	}


	ConstructorHelpers::FObjectFinder<UMaterial> YellowMaterialLoader(TEXT("/Script/Engine.Material'/Game/JSH/BP/Gizmo/MM_Gizmo_Yellow.MM_Gizmo_Yellow'"));
	if (YellowMaterialLoader.Succeeded())
	{
		YellowMaterial = YellowMaterialLoader.Object;
	}

	ConstructorHelpers::FObjectFinder<UMaterial> BlueMaterialLoader(TEXT("/Script/Engine.Material'/Game/JSH/BP/Gizmo/MM_Gizmo_Blue.MM_Gizmo_Blue'"));
	if (BlueMaterialLoader.Succeeded())
	{
		BlueMaterial = BlueMaterialLoader.Object;
	}
}

// Called when the game starts or when spawned
void AJSH_Translate_GizmoZ::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Error, TEXT("Component 2222"));
	
	JPlayerController = Cast<AJSH_PlayerController>(GetWorld()->GetFirstPlayerController());
	OriginPlayer = Cast<AJSH_Player>(JPlayerController->GetPawn());

	
}

// Called every frame
void AJSH_Translate_GizmoZ::Tick(float DeltaTime)
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


void AJSH_Translate_GizmoZ::NotifyActorOnClicked(FKey ButtonPressed)
{
	if (!CursorOveringGizmo) return;
	
	Super::NotifyActorOnClicked(ButtonPressed);
	

	if (OriginPlayer->Editor_SpawnActor->GizmoX_ON) return;
	if (OriginPlayer->Editor_SpawnActor->GizmoY_ON) return;
	if (!OriginPlayer->Editor_SpawnActor->GizmoZ_ON)
	{
		OriginPlayer->Editor_SpawnActor->GizmoZ_ON = true;
	}
	
	UE_LOG(LogTemp, Error, TEXT("z1"));
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
	// Params.AddIgnoredActor(dd);
	
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);
	if (bHit)
	{
		// 다른 축과 겹쳐졌을때 else랑 같이 들어오는 오류가 있씀 
		//if (HitResult.GetActor() != this) return;
	
		//DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 1, 0, 0.3);
		UE_LOG(LogTemp, Error, TEXT("z2"));
		if (!firstclick && !Clicked)
		{
			Clicked = true;
			firstclick = true;
			
			UE_LOG(LogTemp, Error, TEXT("z3"));
			
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
				UE_LOG(LogTemp, Error, TEXT("z4"));
				End_Location = HitResult.ImpactPoint;
				NewLocation = FVector(StartGizmoLocation.X, StartGizmoLocation.Y, End_Location.Z - StartActor_Location.Z);
				OriginPlayer->Editor_SpawnActor->SetActorLocation(NewLocation);
				DuplicateSelected = false;
			// if (DuplicateSelected)
			// {
			// 	UE_LOG(LogTemp, Error, TEXT("z4"));
			// 	End_Location = HitResult.ImpactPoint;
			// 	NewLocation = FVector(StartGizmoLocation.X, StartGizmoLocation.Y, End_Location.Z - StartActor_Location.Z);
			// 	OriginPlayer->Editor_SpawnActor->SetActorLocation(NewLocation);
			// 	DuplicateSelected = false;
			// }
			// else
			// {
			// 	UE_LOG(LogTemp, Error, TEXT("z5"));
			// 	End_Location = End;
			// 	NewLocation = FVector(StartGizmoLocation.X, StartGizmoLocation.Y, End_Location.Z - StartActor_Location.Z);
			// 	OriginPlayer->Editor_SpawnActor->SetActorLocation(NewLocation);
			// }
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("z5"));
		End_Location = End;
		
		NewLocation = FVector(StartGizmoLocation.X, StartGizmoLocation.Y, End_Location.Z - StartActor_Location.Z);
		OriginPlayer->Editor_SpawnActor->SetActorLocation(NewLocation);
		//
		// firstclick = false;
	}
}



// void AJSH_Translate_GizmoZ::NotifyActorOnReleased(FKey ButtonReleased)
// {
// 	Super::NotifyActorOnReleased(ButtonReleased);
//
// 	// Clicked = false;
// 	// SelectedGizmo = false;
// 	OriginColor();
// }


// 오버랩 색상 변경
void AJSH_Translate_GizmoZ::NotifyActorBeginCursorOver()
{
	Super::NotifyActorBeginCursorOver();


	if (OriginPlayer->Editor_SpawnActor->GizmoX_ON) return;
	if (OriginPlayer->Editor_SpawnActor->GizmoY_ON) return;
	
	SelectedColor();
	CursorOveringGizmo = true;
}
void AJSH_Translate_GizmoZ::NotifyActorEndCursorOver()
{
	Super::NotifyActorEndCursorOver();

	if (!Clicked)
	{
		OriginColor();
	}
	// HandleMouseReleaseOutsideActor();
}


void AJSH_Translate_GizmoZ::OriginColor()
{
	if (SelectedGizmo) return;
	
	if (BlueMaterial)
	{
		Origin->SetMaterial(0, BlueMaterial);
	}
	// Selected->SetVisibility(false);
	// Origin->SetVisibility(true);
}

void AJSH_Translate_GizmoZ::SelectedColor()
{
	if (YellowMaterial)
	{
		Origin->SetMaterial(0, YellowMaterial);
	}
	// Selected->SetVisibility(true);
	// Origin->SetVisibility(false);
}

void AJSH_Translate_GizmoZ::EndClick()
{
	Clicked = false;
	SelectedGizmo = false;
	OriginColor();
}


void AJSH_Translate_GizmoZ::HandleMouseReleaseOutsideActor()
{
	Clicked = false;
	firstclick = false;
	DuplicateSelected = true;
	SelectedGizmo = false;
	CursorOveringGizmo = false;
	OriginPlayer->Editor_SpawnActor->GizmoZ_ON = false;
	OriginColor();
}