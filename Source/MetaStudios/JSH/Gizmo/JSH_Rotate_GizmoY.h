// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "JSH_Rotate_GizmoY.generated.h"

class AJSH_PlayerController;
class AJSH_Player;

UCLASS()
class METASTUDIOS_API AJSH_Rotate_GizmoY : public AActor
{
	GENERATED_BODY()

	UPROPERTY()
	USceneComponent* Root_Scene;
	
	UPROPERTY()
	UStaticMeshComponent* Origin;
	UPROPERTY()
	UStaticMeshComponent* Origin2;
	UPROPERTY()
	UStaticMeshComponent* Origin3;
	UPROPERTY()
	UStaticMeshComponent* Origin4;

	
	UPROPERTY()
	UMaterial* YellowMaterial;
	UPROPERTY()
	UMaterial* GreenMaterial;

	// UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Body, meta = (AllowPrivateAccess = "true"))
	// UStaticMeshComponent* Selected;

public:
	// Sets default values for this actor's properties
	AJSH_Rotate_GizmoY();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;


public:
	virtual void NotifyActorOnClicked(FKey ButtonPressed = EKeys::LeftMouseButton);
	// virtual void NotifyActorOnReleased(FKey ButtonReleased);

	UFUNCTION()
	void GOnClicked();



	UPROPERTY()
	AJSH_Player* OriginPlayer;

	UPROPERTY()
	FVector2D MousePosition;
	UPROPERTY()
	FVector Mouse_WorldLocation;
	UPROPERTY()
	FVector Mouse_WorldDirection;
	UPROPERTY()
	FVector Start;
	UPROPERTY()
	FVector End;
	UPROPERTY()
	FHitResult HitResult;
	FCollisionQueryParams Params;
	UPROPERTY()
	FVector NewLocation;
	UPROPERTY()
	FVector NoHit_End_Location = End;
	UPROPERTY()
	float Lay_Distance = 0.0f;
	UPROPERTY()
	bool firstclick = false;
	UPROPERTY()
	FVector StartActor_Location;
	UPROPERTY()
	FVector StartMouselocation;
	UPROPERTY()
	FVector End_Location;
	UPROPERTY()
	FVector StartGizmoLocation;
	UPROPERTY()
	AJSH_PlayerController* JPlayerController;

	UPROPERTY()
	float MouseX;
	UPROPERTY()
	float MouseY;
	UPROPERTY()
	float StartMouselocation_2D_X;
	UPROPERTY()
	float StartMouselocation_2D_Y;
	UPROPERTY()
	float EndMouselocation_2D_X;
	UPROPERTY()
	float EndMouselocation_2D_Y;
	UPROPERTY()
	float End_variation;


	UPROPERTY()
	FRotator Start_Rotate;
	UPROPERTY()
	FRotator End_Rotate;

	UPROPERTY()
	float XDifference;

	// Gizmo 처음 클릭 확인을 위한
	UPROPERTY()
	bool Clicked = false;

	// Gizmo가 클릭된 상태라면 , 마우스가 Gizmo 위에 있지 않아도 계속해서 노란색 유지하기 위해
	UPROPERTY()
	bool SelectedGizmo = false;

	// UFUNCTION()
	// virtual void NotifyActorBeginCursorOver();
	// UFUNCTION()
	// virtual void NotifyActorEndCursorOver();
	UFUNCTION()
	void BeginCursorOver();
	UFUNCTION()
	void EndCursorOver();


	// int32 ConsecutiveHitCount = 0;
	// int32 ConsecutiveMissCount = 0;

	// 색상 관련
	UFUNCTION()
	void OriginColor();
	UFUNCTION()
	void SelectedColor();

	// 초기화 함수
	UFUNCTION()
	void HandleMouseReleaseOutsideActor();

	// Cursor에 오버랩 되었을때 True로 바뀌는 bool값임 , 커서에 마우스 올라가 있을때에만 클릭해도 실행되도록 (왜 넣었는지 기억 안남, 없어도 될듯 싶음)
	bool CursorOveringGizmo = false;

	// UFUNCTION()
	// void FindAndStoreGizmoActors(UWorld* WorldContext);
	// //TArray<class AJSH_Translate_GizmoY*> FoundGizmoActors;
	// UPROPERTY()
	// AJSH_Scale_GizmoX* Gizmo_X;
	// UPROPERTY()
	// AJSH_Translate_GizmoY* Gizmo_Y;



	// Player쪽에서 Gizmo Mode 바꿀때 조정해줌
	UFUNCTION()
	void Visible_and_Collision_On();
	UFUNCTION()
	void Visible_and_Collision_Off();


	UFUNCTION()
	void BeginPlayer(AJSH_Player* temp, AJSH_PlayerController* control);
};