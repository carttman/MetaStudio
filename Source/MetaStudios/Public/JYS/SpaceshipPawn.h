// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MetaStudiosCharacter.h"
#include "SpaceshipPawn.generated.h"

UCLASS()
class METASTUDIOS_API ASpaceshipPawn : public APawn
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SpaceshipMove;	
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SpaceshipLook;	
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SpaceshipMoveUp;	
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SpaceshipMoveDown;

public:
	// Sets default values for this pawn's properties
	ASpaceshipPawn();

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


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* SpaceshipMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USkeletalMeshComponent* SpaceshipSkeletalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UCameraComponent* CameraComp;

	UPROPERTY()
	class USpaceshipAnimInstance* Anim;

	bool CanPlayerEnter(AMetaStudiosCharacter* targetCharacter);

	void ExitSpaceship();

	UFUNCTION(Server, Reliable)
	void Server_ExitSpaceship();

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_ExitSpaceship();

	void ResetEnhancedInputSetting(class APlayerController* pc);

	AMetaStudiosCharacter* player;

	void OnMyActionMove(const FInputActionValue& value);

	void OnMyActionLook(const FInputActionValue& value);

	float speed = 500.0f;

	UFUNCTION(Server, Unreliable)
	void Server_UpdateTransform(FVector newLocation, FRotator newRotation);

	void OnMoveUp(const FInputActionValue& value);
	void OnMoveDown(const FInputActionValue& value);

	UFUNCTION(Server, Reliable)
	void Server_OnMyActionMoveSpaceship(bool bMove);

	void ApplyRoll(float RollInput);
	void ApplyRollBack();
	UPROPERTY(Replicated)
	bool MoveStop = true;

	UFUNCTION(Server, Unreliable)
	void Server_UpdateTransformSpaceship(FVector newLocation, FRotator newRotation);


	float DescentSpeed = 5.0f;

	/////////////////Animation//////////////////////////////
	

	UPROPERTY(EditAnywhere)
	class UAnimSequence* doorAnim;

	UPROPERTY(EditAnywhere)
	float LandingDistance = 500.0f;

	bool bCantMove = false;

	bool bLanded = true;

	////////////////Effect//////////////
	void StartFlyEffect();

	//UPROPERTY(EditAnywhere, Category = "Effects")
	//class UNiagaraComponent* startFlyFXComponent;

	//void ActivateStartFly(bool bActive);

	//UPROPERTY(Replicated)
	//bool MoveStop = true;

	//bool activeStartFly = true;

	UFUNCTION(Server,Reliable)
	void Server_OnMyActionMoveSpaceship(bool bMove);

	void ApplyRoll(float RollInput);
	void ApplyRollBack();

	UPROPERTY(Replicated)
	bool MoveStop = true;

	
	UFUNCTION(Server, Unreliable)
	void Server_SpaceshipUpdateTransform(FVector newLocation, FRotator newRotation);

private:
	
	FVector MovementDirection;

	FVector direction;

	UPROPERTY(EditAnywhere)
	float MovementSpeed = 500.0f;

	////////////라인트레이스를 쏴서 일정 거리 이하가 됐을 때 착지 애니메이션 실행 및 자동 착지/////////////////////
	UPROPERTY(EditAnywhere)
	float landingTriggerDistance = 500.0f;

	// 지면에 닿았다면 착륙 상태로 변환하고 LegDown애니메이션을 실행
	UFUNCTION()
	bool CheckLanding();


	////////Animation////////////////////////////////
	UFUNCTION(Server, Reliable)
	void Server_PlayAnimMontage(class UAnimMontage* montageToPlay, float playRate = 1.0f, FName startSection = NAME_None);

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_PlayAnimMontage(class UAnimMontage* montageToPlay, float playRate = 1.0f, FName startSection = NAME_None);

	UFUNCTION()
	void MoveUpAnim();

	UFUNCTION(Server, Reliable)
	void Server_CloseDoorMontageSetting();

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_CloseDoorMontageSetting();
	////////////Animation/////////////////////////

};
