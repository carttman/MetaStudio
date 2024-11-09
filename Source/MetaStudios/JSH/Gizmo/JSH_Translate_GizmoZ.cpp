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
	//Params.AddIgnoredActor(OriginPlayer->Editor_SpawnActor);
	
	
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);
	if (bHit)
	{
		if (HitResult.GetActor() == this)
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
			
				UE_LOG(LogTemp, Error, TEXT("point %s"), *HitResult.ImpactPoint.ToString());
				UE_LOG(LogTemp, Error, TEXT("gizmo %s"), *StartGizmoLocation.ToString());
				SelectedGizmo = true;
			}
			else
			{
				End_Location = HitResult.ImpactPoint;
			
			
				FVector NewLocation = FVector(StartGizmoLocation.X, StartGizmoLocation.Y, End_Location.Z - StartActor_Location.Z);
				OriginPlayer->Editor_SpawnActor->SetActorLocation(NewLocation);
			
				firstclick = false;
			}
		}
		else
		{
			HandleMouseReleaseOutsideActor();
		}
	}
	else
	{
		//DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 1, 0, 0.3);

		HandleMouseReleaseOutsideActor();
	}
}



void AJSH_Translate_GizmoZ::NotifyActorOnReleased(FKey ButtonReleased)
{
	Super::NotifyActorOnReleased(ButtonReleased);

	Clicked = false;
	SelectedGizmo = false;
	OriginColor();
}


// 오버랩 색상 변경
void AJSH_Translate_GizmoZ::NotifyActorBeginCursorOver()
{
	Super::NotifyActorBeginCursorOver();

	SelectedColor();
}
void AJSH_Translate_GizmoZ::NotifyActorEndCursorOver()
{
	Super::NotifyActorEndCursorOver();

	OriginColor();
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
	SelectedGizmo = false;
	OriginColor();
}
