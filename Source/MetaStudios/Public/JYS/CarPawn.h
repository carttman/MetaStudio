// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MetaStudiosCharacter.h"
#include "../../../../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraComponent.h"
#include "../../../../Plugins/FX/Niagara/Source/Niagara/Classes/NiagaraSystem.h"
#include "CarPawn.generated.h"

UCLASS()
class METASTUDIOS_API ACarPawn : public APawn
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* CarMove;	
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* CarLook;

public:
	// Sets default values for this pawn's properties
	ACarPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// bool값 멀티
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


	bool CanPlayerEnterCar(AMetaStudiosCharacter* targetCharacter);

	void ExitCar();

	UFUNCTION(Server, Reliable)
	void Server_ExitCar();

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_ExitCar();

	AMetaStudiosCharacter* player;

	void OnMyActionLook(const FInputActionValue& value);

	void OnMyActionMove(const FInputActionValue& value);
	UFUNCTION(Server, Reliable)
	void Server_OnMyActionMove(bool bMove);

	void ApplyRoll(float RollInput);
	void ApplyRollBack();
	UPROPERTY(Replicated)
	bool MoveStop = true;


	float carSpeed = 10000.0f;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* CarMesh;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//class USpringArmComponent* SpringArm;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//class UCameraComponent* CameraComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USceneComponent* DefaultScene;


	void ResetEnhancedInputSetting(class APlayerController* pc);

	UFUNCTION(Server, Unreliable)
	void Server_UpdateTransform(FVector newLocation, FRotator newRotation);

	///////////////////이펙트 추가//////////////
	UPROPERTY(EditAnywhere, Category = "Effects")
	class UNiagaraComponent* ThrusterComponent;	
	UPROPERTY(EditAnywhere, Category = "Effects")
	class UNiagaraComponent* ThrusterComponent2;

	//UPROPERTY(EditAnywhere, Category = "Effects")
	//class UNiagaraSystem* ThrusterFXSystem;

	void ActivateThruster(bool bActive);

	bool ActiveThruster = true;

	FVector MovementDirection;

	FVector direction;

	UPROPERTY(EditAnywhere)
	float MovementSpeed = 1000.0f;

	////////////플레이어가 앉아 있는 자세 Visible 껐다 켜기//////////////
	UPROPERTY(EditAnywhere)
	class USkeletalMeshComponent* RidingPlayer;

};
