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
	
	// Tick���� �ν��� ���� �Լ�
	void ManageBooster(float DeltaTime);

	UFUNCTION(Server, Reliable)
	void Server_ManageBooster(float DeltaTime);

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_ManageBooster(float DeltaTime);
 
	// Booster ���
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

	// �߷�
	void GravityScaleOff();

	void GravityScaleOn();

	// ���� ����� ���� ã��
	void FindObject();

	UFUNCTION(Server, Reliable)
	void Server_FindObject();

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_FindObject();

	void DestroyObject();

	// ī�޶� ��ȯ (TPS, FPS)

	void ChangeCameraMode();

	UFUNCTION(Server, Reliable)
	void Server_ChangeCameraMode();

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_ChangeCameraMode();

	void SelectAutoMobile();
	// �÷��̾�� ���ּ� ��Ʈ�ѷ� �ٲٱ�
	//void EnterSpaceship();

	UFUNCTION(Server, Reliable)
	void Server_EnterSpaceship(class AActor* TargetActor);

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_EnterSpaceship(class ASpaceshipPawn* SpaceshipActor);

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<APawn> SpaceshipPawnFactory;

	APlayerController* MetaController;

	//// �÷��̾�� �ڵ����� ��Ʈ�ѷ� �ٲٱ�
	//void EnterCar();

	UFUNCTION(Server, Reliable)
	void Server_EnterCar(class AActor* TargetActor);

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_EnterCar(class ACarPawn* CarActor);

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<APawn> CarPawnFactory;

	// bool�� ��Ƽ
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(EditAnywhere)
	class ACarPawn* carActor;

	UPROPERTY(EditAnywhere)
	class ASpaceshipPawn* spaceshipActor;

	///////////////////����Ʈ �߰�//////////////
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
	// �ν��� ��� ����
	UPROPERTY(ReplicatedUsing = UsingBooster)
	bool bIsBoosting = false;

	// �ν��� ��
	float BoostStrength = 50000.0f;

	// �ν����� �ִ� ��
	float MaxBoosterAmount = 1000.0f;

	// ���� �ν��� ��
	float BoosterAmount = 1000.0f;

	// �ν��� ���� �ӵ�
	float BoosterDrainRate = 20.0f;

	// �ν��� ȸ�� �ӵ�
	float BoosterRefillRate = 10.0f;

	// �⺻ �߷� ��
	float GravityScaleNormal = 1.0f;  
	
	// �߷��� ������ �� ����� ��
	float GravityScaleZero = 0.0f;    

	AActor* nearActor;

	// TPS / FPS ī�޶� ��ȯ
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



	// �� ���
	UPROPERTY()
	TArray<ACarPawn*> ListCar;
	// ���ּ� ���
	UPROPERTY()
	TArray<ASpaceshipPawn*>	ListShip;	

	
	// ����Ͽ�  add���ִ°�
	// ���ּ� ��Ͽ� add���ִ°�
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

