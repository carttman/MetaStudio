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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_Exit;


public:
	AMetaStudiosCharacter();

	virtual void Tick(float DeltaTime) override;

	virtual void PossessedBy(AController* NewController) override;

	//UPROPERTY(EditAnywhere)
	//class UAudioComponent* FootstepAudioComp;

	//UPROPERTY()
	//bool bAutoActivate;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* BoosterPackMesh;

	/*void AttachBoosterPack() const;*/
	
	// Tick에서 부스터 관리 함수
	void ManageBooster(float DeltaTime);

	UFUNCTION(Server, Reliable)
	void Server_ManageBooster(float DeltaTime);

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_ManageBooster(float DeltaTime);
 
	// Booster 사용
	void ToggleBoosting();
	
	UFUNCTION(Server, Reliable)
	void Server_ToggleBoosting(bool bMove);

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

	void SelectAutoMobile();
	// 플레이어랑 우주선 컨트롤러 바꾸기
	//void EnterSpaceship();

	UFUNCTION(Server, Reliable)
	void Server_EnterSpaceship(class AActor* TargetActor);

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_EnterSpaceship(class ASpaceshipPawn* SpaceshipActor);

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<APawn> SpaceshipPawnFactory;

	APlayerController* MetaController;

	//// 플레이어랑 자동차랑 컨트롤러 바꾸기
	//void EnterCar();

	UFUNCTION(Server, Reliable)
	void Server_EnterCar(class AActor* TargetActor);

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_EnterCar(class ACarPawn* CarActor);

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<APawn> CarPawnFactory;

	// bool값 멀티
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(EditAnywhere)
	class ACarPawn* carActor;

	UPROPERTY(EditAnywhere)
	class ASpaceshipPawn* spaceshipActor;

	///////////////////이펙트 추가//////////////
	UPROPERTY(EditAnywhere, Category = "Effects")
	class UNiagaraComponent* BoosterFXComponent;	
	
	UPROPERTY(EditAnywhere, Category = "Effects")
	class UNiagaraComponent* BoosterFXComponent2;

	UPROPERTY(EditAnywhere, Category = "Effects")
	class UNiagaraComponent* BoosterFXComponent3;

	UPROPERTY(EditAnywhere)
	class UArrowComponent* BoosterArrow1;

	void ActivateBooster(bool bActive);
	
	UPROPERTY()
	bool MoveStop = true;

	bool activeBooster = true;


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

	UPROPERTY(EditDefaultsOnly , Category = Pistol)
	float GrabDistance = 300;

	UFUNCTION()
	void UsingBooster();
private:
	// 부스터 사용 여부
	UPROPERTY(ReplicatedUsing = UsingBooster)
	bool bIsBoosting = false;

	// 부스터 힘
	float BoostStrength = 50000.0f;

	// 부스터의 최대 값
	float MaxBoosterAmount = 1000.0f;

	// 현재 부스터 양
	float BoosterAmount = 1000.0f;

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

	////////////Animation//////////////////

	UPROPERTY()
	class UPlayerAnimInstance* Anim;

	UFUNCTION(Server, Reliable)
	void Server_PlayAnimMontage(class UAnimMontage* montageToPlay, float playRate = 1.0f, FName startSection = NAME_None);

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_PlayAnimMontage(class UAnimMontage* montageToPlay, float playRate = 1.0f, FName startSection = NAME_None);

	UFUNCTION()
	void PickUpAnim();

	void ExitSession();



	// 차 목록
	UPROPERTY()
	TArray<ACarPawn*> ListCar;
	// 우주선 목록
	UPROPERTY()
	TArray<ASpaceshipPawn*>	ListShip;	

	
	// 차목록에  add해주는거
	// 우주선 목록에 add해주는거
	UFUNCTION()
	void AddVehicle(class AActor* target);
	//void AddVehicle(class ASpaceshipPawn* target);
	//UFUNCTION()
	//void AddVehicle(class ACarPawn* target);
	
	UFUNCTION()
	void DeleteVehicle(class AActor* target);
	//void DeleteVehicle(class ASpaceshipPawn* target);
	//UFUNCTION()
	//void DeleteVehicle(class ACarPawn* target);


public:
	UFUNCTION()
	bool IsLocallyPlayer()	{	return IsLocallyControlled(); }

};

