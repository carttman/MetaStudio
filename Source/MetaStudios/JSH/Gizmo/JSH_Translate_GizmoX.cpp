// Fill out your copyright notice in the Description page of Project Settings.


#include "../../JSH/Gizmo/JSH_Translate_GizmoX.h"
#include "MetaStudios/JSH/JSH_PlayerController.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "Components/PrimitiveComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/EngineTypes.h" 
#include "MetaStudios/JSH/JSH_Editor_SpawnActor.h"


// Sets default values
AJSH_Translate_GizmoX::AJSH_Translate_GizmoX()
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
	
	
	ConstructorHelpers::FObjectFinder<UMaterial> OriginMaterial(TEXT("/Script/Engine.Material'/Game/JSH/BP/Gizmo/MM_Gizmo_Red.MM_Gizmo_Red'"));
	if (OriginMaterial.Succeeded())
	{
		Origin->SetMaterial(0, OriginMaterial.Object);
	}


	ConstructorHelpers::FObjectFinder<UMaterial> YellowMaterialLoader(TEXT("/Script/Engine.Material'/Game/JSH/BP/Gizmo/MM_Gizmo_Yellow.MM_Gizmo_Yellow'"));
	if (YellowMaterialLoader.Succeeded())
	{
		YellowMaterial = YellowMaterialLoader.Object;
	}

	ConstructorHelpers::FObjectFinder<UMaterial> RedMaterialLoader(TEXT("/Script/Engine.Material'/Game/JSH/BP/Gizmo/MM_Gizmo_Red.MM_Gizmo_Red'"));
	if (RedMaterialLoader.Succeeded())
	{
		RedMaterial = RedMaterialLoader.Object;
	}

	
	//
	// // 선택되었을 때 노란색
	// Selected = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Selected"));
	// Selected->SetupAttachment(RootComponent);
	// ConstructorHelpers::FObjectFinder<UStaticMesh> TMesh2(TEXT("/Script/Engine.StaticMesh'/Game/JSH/BP/Gizmo/Translate_Select/SM_TranslationHandle_X.SM_TranslationHandle_X'"));
	// if (TMesh2.Succeeded())
	// {
	// 	Selected->SetStaticMesh(TMesh2.Object);
	// 	Selected->SetVisibility(false);
	// 	//Origin->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, -90, 0));
	// 	//Origin->SetRelativeScale3D(FVector(0.5, 0.5, 0.5));
	// }
}

// Called when the game starts or when spawned
void AJSH_Translate_GizmoX::BeginPlay()
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
void AJSH_Translate_GizmoX::Tick(float DeltaTime)
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
	//
	// if (Clicked == true && OriginPlayer->Clicked == false)
	// {
	// 	EndClick();
	// }
}


void AJSH_Translate_GizmoX::NotifyActorOnClicked(FKey ButtonPressed)
{
	Super::NotifyActorOnClicked(ButtonPressed);
	// 마우스 2D -> 3D Vector 변환
	if (JPlayerController->GetMousePosition(MousePosition.X, MousePosition.Y))
	{
		JPlayerController->DeprojectMousePositionToWorld(Mouse_WorldLocation, Mouse_WorldDirection);
	}
	

	FVector Start = Mouse_WorldLocation;
	FVector End =  (Mouse_WorldDirection * 10000.0f) + Mouse_WorldLocation;
	
	FHitResult HitResult;
	FCollisionQueryParams Params;
	//Params.AddIgnoredActor(this);
	
	
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
			// Start_Mouse_WorldLocation = HitResult.Location.X;
			
			StartMouselocation = HitResult.ImpactPoint;
			StartGizmoLocation = OriginPlayer->Editor_SpawnActor->GizmoActor->GetActorLocation();
			StartActor_Location = StartMouselocation - StartGizmoLocation;
			float GapX = StartMouselocation.X - StartGizmoLocation.X;
			UE_LOG(LogTemp, Error, TEXT("point %s"), *HitResult.ImpactPoint.ToString());
			UE_LOG(LogTemp, Error, TEXT("gizmo %s"), *StartGizmoLocation.ToString());
			SelectedGizmo = true;
		}
		else
		{
			End_Location = HitResult.ImpactPoint;
			FVector see = StartMouselocation - End_Location;
			
			FVector NewLocation = FVector(End_Location.X - StartActor_Location.X, StartGizmoLocation.Y, StartGizmoLocation.Z);
			OriginPlayer->Editor_SpawnActor->SetActorLocation(NewLocation);
			
			
			
			firstclick = false;
		}
	}
	else
	{
		DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 1, 0, 0.3);
	}
}



void AJSH_Translate_GizmoX::NotifyActorOnReleased(FKey ButtonReleased)
{
	Super::NotifyActorOnReleased(ButtonReleased);

	Clicked = false;
	SelectedGizmo = false;
	OriginColor();
}


// 오버랩 색상 변경
void AJSH_Translate_GizmoX::NotifyActorBeginCursorOver()
{
	Super::NotifyActorBeginCursorOver();

	SelectedColor();
}
void AJSH_Translate_GizmoX::NotifyActorEndCursorOver()
{
	Super::NotifyActorEndCursorOver();

	OriginColor();
}


void AJSH_Translate_GizmoX::OriginColor()
{
	if (SelectedGizmo) return;
	
	if (RedMaterial)
	{
		Origin->SetMaterial(0, RedMaterial);
	}
	// Selected->SetVisibility(false);
	// Origin->SetVisibility(true);
}

void AJSH_Translate_GizmoX::SelectedColor()
{
	if (YellowMaterial)
	{
		Origin->SetMaterial(0, YellowMaterial);
	}
	// Selected->SetVisibility(true);
	// Origin->SetVisibility(false);
}

void AJSH_Translate_GizmoX::EndClick()
{
	Clicked = false;
	SelectedGizmo = false;
	OriginColor();
}


void AJSH_Translate_GizmoX::HandleMouseReleaseOutsideActor()
{
	Clicked = false;
	SelectedGizmo = false;
	OriginColor();
}