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

	virtual void PossessedBy(AController* NewController) override;
	
	// Tick에서 부스터 관리 함수
	void ManageBooster(float DeltaTime);

	UFUNCTION(Server, Reliable)
	void Server_ManageBooster(float DeltaTime);

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_ManageBooster(float DeltaTime);
 
	// Booster 사용
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

	void ResetEnhancedInputSetting(class APlayerController* pc);

	// 중력
	void GravityScaleOff();

	void GravityScaleOn();

	// 가장 가까운 액터 찾기
	void FindObject();

	UFUNCTION(Server, Reliable)
	void Server_FindObject();

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_FindObject();

	void DestroyObject();

	// 카메라 전환 (TPS, FPS)

	void ChangeCameraMode();

	UFUNCTION(Server, Reliable)
	void Server_ChangeCameraMode();

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_ChangeCameraMode();

	// 플레이어랑 우주선 컨트롤러 바꾸기
	void EnterSpaceship();

	UFUNCTION(Server, Reliable)
	void Server_EnterSpaceship();

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_EnterSpaceship(class ASpaceshipPawn* SpaceshipActor);

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<APawn> SpaceshipPawnFactory;	

	APlayerController* MetaController;

	//// 플레이어랑 자동차랑 컨트롤러 바꾸기
	void EnterCar();

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<APawn> CarPawnFactory;

	// bool값 멀티
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

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


private:
	// 부스터 사용 여부
	UPROPERTY(Replicated)
	bool bIsBoosting = false;

	// 부스터 힘
	float BoostStrength = 2000.0f;

	// 부스터의 최대 값
	float MaxBoosterAmount = 100.0f;

	// 현재 부스터 양
	float BoosterAmount = 100.0f;

	// 부스터 감소 속도
	float BoosterDrainRate = 20.0f;

	// 부스터 회복 속도
	float BoosterRefillRate = 10.0f;

	// 기본 중력 값
	float GravityScaleNormal = 1.0f;  
	
	// 중력을 제거할 때 사용할 값
	float GravityScaleZero = 0.0f;    

	AActor* nearActor;

	// TPS / FPS 카메라 전환
	UPROPERTY(Replicated)
	bool IsTPSMode = false;

	FTimerHandle CameraModeTimerHandle;
	
	float CameraModeTime;

	float CameraBlendTime = 0;


};

