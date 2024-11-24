// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "JSH_Translate_GizmoX.h"
#include "GameFramework/Actor.h"
#include "MetaStudios/JSH/JSH_PlayerController.h"
#include "JSH_Gizmo.generated.h"


// class AJSH_Editor_SpawnActor;
class AJSH_PlayerController;
class AJSH_Player;
class AJSH_Translate_GizmoBox;
class AJSH_Translate_GizmoX;
class AJSH_Translate_GizmoY;
class AJSH_Translate_GizmoZ;
class AJSH_Scale_GizmoBox;
class AJSH_Scale_GizmoX;

UCLASS()
class METASTUDIOS_API AJSH_Gizmo : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USceneComponent* RootScence;

	// Translate
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	// USceneComponent* Translate_RootScence;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UChildActorComponent* Translate_Box;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class AJSH_Translate_GizmoBox* Origin_Translate_Box;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UChildActorComponent* Translate_X;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	AJSH_Translate_GizmoX* Origin_Translate_X;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UChildActorComponent* Translate_Y;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	AJSH_Translate_GizmoY* Origin_Translate_Y;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UChildActorComponent* Translate_Z;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	 AJSH_Translate_GizmoZ* Origin_Translate_Z;
	
	// Sclae
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UChildActorComponent* Scale_Box;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	AJSH_Scale_GizmoBox* Origin_Scale_Box;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UChildActorComponent* Scale_X;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	AJSH_Scale_GizmoX* Origin_Scale_X;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UChildActorComponent* Scale_Y;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	AJSH_Scale_GizmoY* Origin_Scale_Y;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UChildActorComponent* Scale_Z;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	AJSH_Scale_GizmoZ* Origin_Scale_Z;


	// Rotate
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USceneComponent* Rotate_Box;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	AJSH_Rotate_GizmoX* Origin_Rotate_Box;

	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UChildActorComponent* Rotate_X;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	AJSH_Rotate_GizmoX* Origin_Rotate_X;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UChildActorComponent* Rotate_Y;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	AJSH_Rotate_GizmoY* Origin_Rotate_Y;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UChildActorComponent* Rotate_Z;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	AJSH_Rotate_GizmoZ* Origin_Rotate_Z;
	
public:	
	// Sets default values for this actor's properties
	AJSH_Gizmo();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	
	
	FVector2D MousePosition;
	FVector Mouse_WorldLocation;
	FVector Mouse_WorldDirection;

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
	
	// UFUNCTION()
	// virtual void NotifyActorBeginCursorOver();
	// UFUNCTION()
	// virtual void NotifyActorEndCursorOver();
	
	// UPROPERTY()
	// AActor* GizmoActor;

	bool Clicked = false;


\

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scaling")
	float MinDistance = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scaling")
	float MaxDistance = 4000.0f;

	FVector InitialScale;


	// Director에 자신의 정보를 저장하기 위해서
	UPROPERTY()
	AJSH_Player* OriginPlayer;

	UFUNCTION()
	void BeginPlayerContorller(AJSH_PlayerController* temp);

	UFUNCTION()
	void Begin_PlayerData(AJSH_Player* temp, AJSH_PlayerController* control);


	UFUNCTION()
	void Child_Actor_Detect();

	TArray<AActor*> Tag_X;
	TArray<AActor*> Tag_Y;
	TArray<AActor*> Tag_Z;
	TArray<AActor*> Tag_Box;

	TArray<AActor*> Tag_SX;
	TArray<AActor*> Tag_SY;
	TArray<AActor*> Tag_SZ;
	TArray<AActor*> Tag_SBox;
	
	TArray<AActor*> Tag_RX;
	TArray<AActor*> Tag_RY;
	TArray<AActor*> Tag_RZ;
};
