#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "JSH_Editor_SpawnActor.generated.h"

UCLASS()
class METASTUDIOS_API AJSH_Editor_SpawnActor : public AActor
{
	GENERATED_BODY()
	
public:
	// Static mesh component for the actor
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UStaticMeshComponent* AssetMesh;

	// Constructor
	AJSH_Editor_SpawnActor();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Function to handle the OnClicked event
	UFUNCTION()
	void OnMeshClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed);
};
