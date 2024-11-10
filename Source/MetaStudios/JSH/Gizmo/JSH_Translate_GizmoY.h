// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "JSH_Translate_GizmoY.generated.h"

class AJSH_PlayerController;
class AJSH_Player;

UCLASS()
class METASTUDIOS_API AJSH_Translate_GizmoY : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Body, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Origin;
	
	// UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Body, meta = (AllowPrivateAccess = "true"))
	// UStaticMeshComponent* Selected;
	
public:	
	// Sets default values for this actor's properties
	AJSH_Translate_GizmoY();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	
public:
	virtual void NotifyActorOnClicked(FKey ButtonPressed = EKeys::LeftMouseButton);
	// virtual void NotifyActorOnReleased(FKey ButtonReleased);


	
	FVector2D MousePosition;
	FVector Mouse_WorldLocation;
	FVector Mouse_WorldDirection;


	FVector Start;
	FVector End;
	FHitResult HitResult;
	FCollisionQueryParams Params;
	FVector NewLocation;

	FVector NoHit_End_Location = End;


	float Lay_Distance = 0.0f;

	float Start_Mouse_WorldLocation;
	float End_Mouse_WorldLocation;
	float gizmogo;
	
	bool firstclick = false;


	FVector StartActor_Location;

	
	FVector StartMouselocation;
	FVector End_Location;

	FVector StartGizmoLocation;
	
	UPROPERTY()
	AJSH_PlayerController* JPlayerController;
	
	UFUNCTION()
	virtual void NotifyActorBeginCursorOver();
	UFUNCTION()
	virtual void NotifyActorEndCursorOver();
	
	// UPROPERTY()
	// AActor* GizmoActor;

	bool Clicked = false;

	int32 ConsecutiveHitCount = 0;
	int32 ConsecutiveMissCount = 0;
	
	UFUNCTION()
	void OriginColor();

	UFUNCTION()
	void SelectedColor();

	UPROPERTY()
	UMaterial* YellowMaterial;

	UPROPERTY()
	UMaterial* GreenMaterial;


	UPROPERTY()
	AJSH_Player* OriginPlayer;

	bool SelectedGizmo = false;



	void HandleMouseReleaseOutsideActor();

	bool CursorOveringGizmo = false;

};