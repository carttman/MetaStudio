// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "MetaStudiosCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class AMetaStudiosCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* TPSCameraSpringArm;

	/** Follow camera */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* TPSCamera;

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* FPSCameraSpringArm;

	/** Follow camera */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FPSCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;	
	
	// Booster Input Action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* BoosterAction;	
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* GetObjectAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* CameraMode;

public:
	AMetaStudiosCharacter();

	virtual void Tick(float DeltaTime) override;
	
	// Tick���� �ν��� ���� �Լ�
	void ManageBooster(float DeltaTime);

	UFUNCTION(Server, Reliable)
	void Server_ManageBooster(float DeltaTime);

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_ManageBooster(float DeltaTime);
 
	// Booster ���
	void ToggleBoosting();
	
	UFUNCTION(Server, Reliable)
	void Server_ToggleBoosting();

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_ToggleBoosting();

	void ToggleBoosting_Complete();

	UFUNCTION(Server, Reliable)
	void Server_ToggleBoosting_Complete();

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_ToggleBoosting_Complete();

	// �߷�
	void GravityScaleOff();

	void GravityScaleOn();

	// ���� ����� ���� ã��
	void FindObject();

	void DestroyObject();

	// ī�޶� ��ȯ (TPS, FPS)
	//UPROPERTY(EditDefaultsOnly)
	//class USpringArmComponent* TPSSpringArm;	
	//
	//UPROPERTY(EditDefaultsOnly)
	//class USpringArmComponent* FPSSpringArm;

	//UPROPERTY(EditDefaultsOnly)
	//class UChildActorComponent* TPSCamera;

	//UPROPERTY(EditDefaultsOnly)
	//class UChildActorComponent* FPSCamera;

	void ChangeCameraMode();


	// bool�� ��Ƽ
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> object;

	UPROPERTY(EditAnywhere)
	TArray<AActor*> objects;

	float nearsetObjectDist = 1000.0f;

	AActor* targetObject;

	// int32 itemCount = 0;

public:
	/** Returns CameraBoom subobject **/
	// FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	// FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

private:
	// �ν��� ��� ����
	UPROPERTY(Replicated)
	bool bIsBoosting = false;

	// �ν��� ��
	float BoostStrength = 2000.0f;

	// �ν����� �ִ� ��
	float MaxBoosterAmount = 100.0f;

	// ���� �ν��� ��
	float BoosterAmount = 100.0f;

	// �ν��� ���� �ӵ�
	float BoosterDrainRate = 20.0f;

	// �ν��� ȸ�� �ӵ�
	float BoosterRefillRate = 10.0f;

	// �⺻ �߷� ��
	float GravityScaleNormal = 1.0f;  
	
	// �߷��� ������ �� ����� ��
	float GravityScaleZero = 0.0f;    

	AActor* nearActor;

	UPROPERTY(Replicated)
	bool IsTPSMode = false;

	FTimerHandle CameraModeTimerHandle;
	
	float CameraModeTime;

	float CameraBlendTime = 0;
};

