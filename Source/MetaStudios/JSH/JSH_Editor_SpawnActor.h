#pragma once

#include <stack>
#include "Gizmo/JSH_Gizmo.h"
#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "JSH_Editor_SpawnActor.generated.h"

// 순환참조 문제로, Include 말고
class AJSH_PlayerController;
class AJSH_Gizmo;

UCLASS()
class METASTUDIOS_API AJSH_Editor_SpawnActor : public APawn
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

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;



	UFUNCTION()
	virtual void NotifyActorOnClicked(FKey ButtonPressed = EKeys::LeftMouseButton);
	UFUNCTION(Server, reliable)
	void Server_NotifyActorOnClicked();
	UFUNCTION(NetMulticast, Reliable)
	void NetMulti_NotifyActorOnClicked();


	
	UFUNCTION()
	void Get_PlayerController();
	UFUNCTION(Server, reliable)
	void Server_Get_PlayerController();
	UFUNCTION(NetMulticast, Reliable)
	void NetMulti_Get_PlayerController();


	UPROPERTY(Replicated)
	TArray<AActor*> OriginPlayer_Box;
	
	

	UFUNCTION()
	void Onclicked();
	UFUNCTION()
	void Unclicked();
	
	UPROPERTY(Replicated)
	AJSH_PlayerController* JPlayerController;
	

	// Director에 자신의 정보를 저장하기 위해서
	UPROPERTY(Replicated)
	class AJSH_Player* OriginPlayer;


	UFUNCTION()
	void GizmoSpawn();

	UPROPERTY(Replicated, EditDefaultsOnly, Category = "Gizmo")
	TSubclassOf<AActor> GizmoClass;

	
	UFUNCTION()
	void Gizmo_Spawn();

	UPROPERTY(Replicated)
	FTransform ThisTransform;

	UPROPERTY(Replicated)
	AActor* GizmoActor = nullptr;

	UPROPERTY(Replicated)
	AJSH_Gizmo* OriginGizmo;
	
	UFUNCTION()
	void DestroyThis();


	// Gizmo 중복 선택 막기
	UPROPERTY(Replicated)
	bool GizmoX_ON = false;
	UPROPERTY(Replicated)
	bool GizmoY_ON = false;
	UPROPERTY(Replicated)
	bool GizmoZ_ON = false;
	UPROPERTY(Replicated)
	bool GizmoB_ON = false;


	
	// SpawnActor 이전 위치 저장

	
	UFUNCTION()
	void ReturnPreviousLocation();
	UFUNCTION()
	void AddPreviousLocation(const FVector& newLocation);

	
	std::stack<FVector> PreviousLocations;


	
	const int MaxLocations = 5;





	/// Location 변경 함수
	UFUNCTION()
	void Set_Location_from_Gizmo(FVector Location);
	UFUNCTION(server, reliable)
	void Server_Set_Location_from_Gizmo(FVector Location);
	UFUNCTION(netmulticast, reliable)
	void NetMulti_Set_Location_from_Gizmo(FVector Location);

	
	/// Scale 변경 함수
	UFUNCTION()
	void Set_Scale_from_Gizmo(FVector Scale);
	UFUNCTION(server, reliable)
	void Server_Set_Scale_from_Gizmo(FVector Scale);
	UFUNCTION(netmulticast, reliable)
	void NetMulti_Set_Scale_from_Gizmo(FVector Scale);
};

