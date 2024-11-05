#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "JSH_Editor_SpawnActor.generated.h"

// 순환참조 문제로, Include 말고
class AJSH_PlayerController;

UCLASS()
class METASTUDIOS_API AJSH_Editor_SpawnActor : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
	USceneComponent* RootScence;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UStaticMeshComponent* AssetMesh;
	
	AJSH_Editor_SpawnActor();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;


	UFUNCTION()
	void OnMeshClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed);
	virtual void NotifyActorOnClicked(FKey ButtonPressed = EKeys::LeftMouseButton);

	UPROPERTY()
	AJSH_PlayerController* JPlayerController;
	

	// Director에 자신의 정보를 저장하기 위해서
	UPROPERTY()
	class AJSH_Player* OriginPlayer;


	UFUNCTION()
	void GizmoSpawn();

	FTransform ThisTransform;
	
	AActor* GizmoActor = nullptr;


	UFUNCTION()
	void DestroyThis();
};
