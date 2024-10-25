// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "IWebSocket.h"
#include "JSH_Player.generated.h"


class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

// 순환참조 문제로, Include 말고
class AJSH_PlayerController;


//DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class AJSH_Player : public ACharacter
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* RecordCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Body, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* FallGuys;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Body, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* FallGuys_Camera;
	
	


	// Input
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* StartRecord;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SpectatorModeOnOff;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_EditorMode;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_Up_Down;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_Up_Down2;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_Camera_Spawn_Destroy;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_Camera_Third_First;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_EditMode;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_MouseWheel;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_ZOOM_In;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_ZOOM_Out;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_ZOOM_Default;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_Camera_Right;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_Camera_Left;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_Camera_Default;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_Camera_Zoom_LeftMouse;
public:
	AJSH_Player();
	

protected:

	void Move(const FInputActionValue& Value);
	
	void Look(const FInputActionValue& Value);
			

protected:

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	virtual void BeginPlay();

	virtual void Tick(float DeltaTime) override;

public:

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }



public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


#pragma region Record

	// 녹화 시작 / 종료
	class UJSH_OBSWebSocket* ObsGamInstance;
	FProcHandle PH;

	UFUNCTION()
	void StartRecording();

	UFUNCTION(NetMulticast, Reliable)
	void NetMulti_StartRecording();

	UPROPERTY(Replicated)
	bool Record_b_On_Off = false;


	// 3인칭 <-> 1인칭
	UPROPERTY(Replicated)
	bool Camera_b_Third_First = false;
	UFUNCTION()
	void Camera_Third_First_Change();
	UFUNCTION(NetMulticast, Reliable)
	void NetMulti_Camera_Third_First_Change();

	// 카메라 Visible 껏다 키기
	UPROPERTY(Replicated)
	bool CameraSpawn_b_On_Off = false;
	UFUNCTION()
	void CameraSpawn();
	UFUNCTION(NetMulticast, Reliable)
	void NetMulti_CameraSpawn();
	
#pragma endregion

	
#pragma region Editor (Player <-> SpectatorPawn)
	
	UFUNCTION()
	void SpectatorMode();
	UFUNCTION(NetMulticast, Reliable)
	void NetMulti_SpectatorMode();
	

	UFUNCTION()
	void Visible_On_OFF();
	UFUNCTION(NetMulticast, Reliable)
	void NetMulti_Visible_On_OFF();
	UPROPERTY(Replicated)
	bool PlayerVisible_b_On = true;

	
	AJSH_PlayerController* JPlayerController;
	
#pragma endregion

	
#pragma region FlyMode
	
	UFUNCTION()
	void FlyMode();
	UFUNCTION(NetMulticast, Reliable)
	void NetMulti_FlyMode();
	UPROPERTY(Replicated)
	bool FlyMode_b_On_Off = false;


	UFUNCTION()
	void Fly_Up_Down(const FInputActionValue& Value);
	UFUNCTION(NetMulticast, Reliable)
	void NetMulti_Fly_Up_Down(const FInputActionValue& Value);

	float Fly_Zvalue = 0;
	float Fly_Off_Value = 0 ;



	UFUNCTION()
	void Fly_Down_Ray(const FInputActionValue& Value);
	UFUNCTION(NetMulticast, Reliable)
	void NetMulti_Fly_Down_Ray(const FInputActionValue& Value);


	// 마우스 휠로 FlyMode Speed 조정
	UPROPERTY()
	float MaxFlySpeed_C = 500.f;
	UPROPERTY()
	float BrakingDecelerationFlying_C = 5000.f;

	
	UFUNCTION()
	void FlySpeed(const FInputActionValue& Value);
	
#pragma endregion


#pragma region EditorMode

	UFUNCTION()
	void EditorMode();
	UFUNCTION(NetMulticast, Reliable)
	void NetMulti_EditorMode();

	
	APlayerController* OriginController;
	

	UFUNCTION()
	void EnableEdit();
	UFUNCTION()
	void DisableEdit();

	UPROPERTY(Replicated)
	bool EditorMode_B = false;

#pragma endregion


#pragma region MainPlatform

	// 메인 플랫폼 일떄 기능 막아두는 bool 값
	UPROPERTY(Replicated)
	bool Bool_MainLock = false;
	
#pragma endregion


#pragma region Camera Control

	UPROPERTY()
	bool Bool_ZoomMode = false;
	
	UFUNCTION()
	void Camera_Zoom_In();
	UFUNCTION()
	void Camera_Zoom_Out();
	UFUNCTION()
	void Camera_Zoom_Default();


	UFUNCTION()
	void CameraRight();
	UFUNCTION()
	void CameraLeft();
	UFUNCTION()
	void CameraDefault();
	UFUNCTION()
	void CameraReset();
	
	FRotator DefaultCameraleaning = FRotator(0, 0, 0);
	FRotator NewCameraRotation;
	float Amount = 0.1f;
	float CurrentAngl = 0.0f;

	UPROPERTY()
	float ZoomFOV = 0;
	
	UPROPERTY()
	float ZoomSpeed = 0.5f; 

	UPROPERTY()
	float MinFOV = 120.0f;

	UPROPERTY()
	float MaxFOV = 10.0f;

	bool WheelOn = false;

	UPROPERTY()
	bool Bool_RightZoom = false;
#pragma endregion
	
};