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

	UPROPERTY()
	AMetaStudiosCharacter* player;

	UPROPERTY()
	bool bExistRider = false;

	void OnMyActionMove(const FInputActionValue& value);

	void OnMyActionLook(const FInputActionValue& value);

	float speed = 50000.0f;

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
	
	/*UPROPERTY(Replicated)
	bool effect = false;*/

	UFUNCTION(Server, Unreliable)
	void Server_UpdateTransformSpaceship(FVector newLocation, FRotator newRotation);


	float DescentSpeed = 5.0f;

	// 가까이 가면 키 관련 UI 생성
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UBoxComponent* UIBox;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> WidgetClass;

	UUserWidget* ActiveWidget;

	UFUNCTION()
    void OnUIBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
                             UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
                             bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnUIBoxEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                           UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	// 가까이 가면 키 관련 UI 생성

	/////////////////Animation//////////////////////////////
	

	UPROPERTY(EditAnywhere)
	class UAnimSequence* doorAnim;

	UPROPERTY(EditAnywhere)
	float LandingDistance = 500.0f;

	bool bCantMove = false;

	bool bLanded = true;

	// 착륙 후 player로 possess할 수 있게끔
	bool bIsMoving = false;

	////////////////Effect//////////////

	UFUNCTION(Server, Reliable)
	void Server_StartFlyEffect();

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_StartFlyEffect();

	void EndFlyEffect();

	UFUNCTION(Server, Reliable)
	void Server_EndFlyEffect();

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_EndFlyEffect();

	UPROPERTY(EditAnywhere, Category = "Effects")
	class UNiagaraComponent* startFlyFXComponent;

	UPROPERTY(EditAnywhere, Category = "Effects")
	class UNiagaraComponent* startFlyFXComponent2;

	void ActivateStartFly(bool bActive);

	bool activeStartFly = true;

private:
	
	FVector MovementDirection;

	FVector direction;

	float LastLandingPosZ = 10000000.0f;

	UPROPERTY(EditAnywhere)
	float MovementSpeed = 1500.0f;

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
