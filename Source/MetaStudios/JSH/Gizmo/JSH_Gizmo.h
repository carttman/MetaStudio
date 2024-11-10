// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "JSH_Gizmo.generated.h"


class AJSH_PlayerController;

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
	UChildActorComponent* Translate_X;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UChildActorComponent* Translate_Y;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UChildActorComponent* Translate_Z;
	
	// Sclae
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UChildActorComponent* Scale_Box;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UChildActorComponent* Scale_X;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UChildActorComponent* Scale_Y;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UChildActorComponent* Scale_Z;
	
public:	
	// Sets default values for this actor's properties
	AJSH_Gizmo();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void TranslateMode();

	UFUNCTION()
	void ScaleMode();
	
	virtual void NotifyActorOnClicked(FKey ButtonPressed = EKeys::LeftMouseButton);
	virtual void NotifyActorOnReleased(FKey ButtonReleased);

	
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


	UFUNCTION()
	void OriginColor();

	UFUNCTION()
	void SelectedColor();

	UPROPERTY()
	UMaterial* YellowMaterial;

	UPROPERTY()
	UMaterial* RedMaterial;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scaling")
	float MinDistance = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scaling")
	float MaxDistance = 4000.0f;

	FVector InitialScale;


};
