// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <queue>
#include <stack>

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Gizmo/JSH_Rotate_GizmoX.h"
#include "Gizmo/JSH_Rotate_GizmoY.h"
#include "Gizmo/JSH_Rotate_GizmoZ.h"
#include "Logging/LogMacros.h"
#include "Widget/JSH_Record_UI.h"
#include "JSH_Player.generated.h"

class AJSH_Gizmo;
class AJSH_Scale_GizmoBox;
class AJSH_Scale_GizmoZ;
class AJSH_Scale_GizmoY;
class AJSH_Scale_GizmoX;
class AJSH_Translate_GizmoY;
class AJSH_Translate_GizmoX;
class AJSH_Translate_GizmoZ;
class AJSH_Translate_GizmoBox;
class AJSH_PlayerController;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class AJSH_Editor_SpawnActor;



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
	

	// 캐릭터 -> 카메라로 기획 바뀐거
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Root_Camera01;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Camera02;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Camera03;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Camera04;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Camera05;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Camera06;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Camera07;

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_Mouse_Sensitive_Down;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_Mouse_Sensitive_Up;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_ESC;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_Del;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_Gizmo_Click;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_Gizmo_SelectMode;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_Gizmo_TranslateMode;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_Gizmo_ScaleMode;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_Gizmo_RotateMode;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_PreviousLocation;

	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_UI_OFF;
	
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

	UFUNCTION()
	void Saved_PlayerController();

	
#pragma region Record

	// 녹화 시작 / 종료
	UPROPERTY()
	class UJSH_OBSWebSocket* ObsGamInstance;

	UPROPERTY()
	class UMainGameInstance* CHJ_Instance;

	
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

	UPROPERTY(replicated)
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
	UPROPERTY()
	float Fly_Zvalue = 0;
	UPROPERTY()
	float Fly_Off_Value = 0 ;



	UFUNCTION()
	void Fly_Down_Ray(const FInputActionValue& Value);
	UFUNCTION(NetMulticast, Reliable)
	void NetMulti_Fly_Down_Ray(const FInputActionValue& Value);


	// 마우스 휠로 FlyMode Speed 조정
	UPROPERTY()
	float MaxFlySpeed_C = 500.f;
	UPROPERTY()
	float BrakingDecelerationFlying_C = 10000.f;
	//float BrakingDecelerationFlying_C = 5000.f;

	
	UFUNCTION()
	void FlySpeed(const FInputActionValue& Value);
	UFUNCTION(NetMulticast, Reliable)
	void NetMulti_FlySpeed(float Value);
	
#pragma endregion


#pragma region EditorMode

	UFUNCTION()
	void EditorMode();
	UFUNCTION(Server, reliable)
	void Server_EditorMode();
	UFUNCTION(NetMulticast, Reliable)
	void NetMulti_EditorMode();

	UPROPERTY()
	APlayerController* OriginController;
	

	UFUNCTION()
	void EnableEdit();
	UFUNCTION(Server, reliable)
	void Server_EnableEdit();
	UFUNCTION(NetMulticast, Reliable)
	void NetMulti_EnableEdit();


	
	UFUNCTION()
	void DisableEdit();
	UFUNCTION(Server, reliable)
	void Server_DisableEdit();
	UFUNCTION(NetMulticast, Reliable)
	void NetMulti_DisableEdit();
	

	UPROPERTY(Replicated)
	bool EditorMode_B = false;

	UPROPERTY(Replicated)
	bool EditorMode_On_B = false;

	// UI
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UUserWidget> UI_Editor_Main;
	UPROPERTY(EditDefaultsOnly)
	class UUserWidget* PlayerMainUI;

	// UI
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UUserWidget> UI_Record_01;
	UPROPERTY(EditDefaultsOnly)
	class UUserWidget* RecordUI_01;

	UPROPERTY()
	UJSH_Record_UI* Origin_RecordUI;
	
	
	// Editor Actor Destroy
	UFUNCTION()
	void CLickAndDel();

	UPROPERTY(Replicated)
	bool ClickedEditorActor = false;

	// EditorActor를 클릭하면 그곳에서 자기 정보를 SaveEditorActor(AJSH_Editor_SpawnActor* ClickedActor) 여기로 전달 후 저장
	UFUNCTION()
	void SaveEditorActor(AJSH_Editor_SpawnActor* ClickedActor);
	UFUNCTION(Server, Reliable)
	void Server_SaveEditorActor(AJSH_Editor_SpawnActor* ClickedActor);
	UFUNCTION(NetMulticast, Reliable)
	void NetMulti_SaveEditorActor(AJSH_Editor_SpawnActor* ClickedActor);

	UPROPERTY(Replicated)
	AJSH_Editor_SpawnActor* Editor_SpawnActor;


	UPROPERTY(Replicated)
	AJSH_Editor_SpawnActor* First_Clicked_SpawnActor = nullptr;
	UPROPERTY(Replicated)
	AJSH_Editor_SpawnActor* Recent_Clicked_SpawnActor = nullptr;

	UPROPERTY(replicated)
	bool DisableEdit_b = true;

	UPROPERTY(replicated)
	bool DisableEdit2_b = false;

#pragma endregion


#pragma region Editor_Gizmo

	// Dell 버튼 누르면 Actor 삭제
	UFUNCTION()
	void EditorAcotorDestroy();
	UFUNCTION(NetMulticast, Reliable)
	void NetMulti_EditorAcotorDestroy();
	UPROPERTY(Replicated)
	bool Bool_EditorActorDestroy = false;

	

	// 각자 Gimo에서 BeginPlay 떄에 함수 호출해서 Player에 자신의 정보 저장
	// UFUNCTION()
	// void Save_Gizmo_TX(AActor* Gizmo_TX);
	// UPROPERTY()
	// AJSH_Translate_GizmoX* Saved_Gizmo_TX;
	//
	// UFUNCTION()
	// void Save_Gizmo_TY(AJSH_Translate_GizmoY* Gizmo_TY);
	// UPROPERTY()
	// AJSH_Translate_GizmoY* Saved_Gizmo_TY;

	UFUNCTION()
	void Save_Gizmo_Parents(AActor* Gizmo_Parents);
	UPROPERTY()
	AJSH_Gizmo* Saved_Gizmo_Parents;
	UFUNCTION()
	void Save_Gizmo_TX(AActor* Gizmo_TX);
	UPROPERTY()
	AJSH_Translate_GizmoX* Saved_Gizmo_TX;
	UFUNCTION()
	void Save_Gizmo_TY(AActor* Gizmo_TY);
	UPROPERTY()
	AJSH_Translate_GizmoY* Saved_Gizmo_TY;
	UFUNCTION()
	void Save_Gizmo_TZ(AActor* Gizmo_TZ);
	UPROPERTY()
	AJSH_Translate_GizmoZ* Saved_Gizmo_TZ;
	UFUNCTION()
	void Save_Gizmo_TB(AActor* Gizmo_TB);
	UPROPERTY()
	AJSH_Translate_GizmoBox* Saved_Gizmo_TB;

	
	UFUNCTION()
	void Save_Gizmo_SX(AActor* Gizmo_SX);
	UPROPERTY()
	AJSH_Scale_GizmoX* Saved_Gizmo_SX;
	UFUNCTION()
	void Save_Gizmo_SY(AActor* Gizmo_SY);
	UPROPERTY()
	AJSH_Scale_GizmoY* Saved_Gizmo_SY;
	UFUNCTION()
	void Save_Gizmo_SZ(AActor* Gizmo_SZ);
	UPROPERTY()
	AJSH_Scale_GizmoZ* Saved_Gizmo_SZ;
	UFUNCTION()
	void Save_Gizmo_SB(AActor* Gizmo_SB);
	UPROPERTY()
	AJSH_Scale_GizmoBox* Saved_Gizmo_SB;

	UFUNCTION()
	void Save_Gizmo_RX(AActor* Gizmo_RX);
	UPROPERTY()
	AJSH_Rotate_GizmoX* Saved_Gizmo_RX;

	UFUNCTION()
	void Save_Gizmo_RY(AActor* Gizmo_RY);
	UPROPERTY()
	AJSH_Rotate_GizmoY* Saved_Gizmo_RY;

	UFUNCTION()
	void Save_Gizmo_RZ(AActor* Gizmo_RZ);
	UPROPERTY()
	AJSH_Rotate_GizmoZ* Saved_Gizmo_RZ;

	
	
	// Gizmo Mode
	UPROPERTY(replicated)
	bool FirstGizmode = false;
	UPROPERTY(replicated)
	bool Gizmo_TranslateMode = false;
	UPROPERTY(replicated)
	bool Gizmo_ScaleMode = false;
	UPROPERTY(replicated)
	bool Gizmo_RotateMode = false;

	UFUNCTION()
	void G_SelecteMode();
	UFUNCTION()
	void G_TranslateMode();
	UFUNCTION()
	void G_SclaeMode();
	UFUNCTION()
	void G_RotateMode();


	UFUNCTION()
	void Gizmo_Detect();
	UPROPERTY()
	FVector2D MousePosition;
	UPROPERTY()
	FVector Mouse_WorldLocation;
	UPROPERTY()
	FVector Mouse_WorldDirection;
	UPROPERTY()
	FVector Start;
	UPROPERTY()
	FVector End;
	UPROPERTY()
	FHitResult HitResult;
	FCollisionQueryParams Params;
	UPROPERTY(Replicated)
	float Lay_Distance = 0.0f;

	UPROPERTY(Replicated)
	bool Gizmo_Selected = false;

	UPROPERTY(Replicated)
	bool EnableEditSystem = false;
	
	UPROPERTY(Replicated)
	bool Gizmo_Detecting = false;


	UFUNCTION()
	void Gizmo_Click();
	UFUNCTION()
	void Gizmo_Click_End();

	UPROPERTY(Replicated)
	bool Gizmo_Clicking_forError = false;

	UPROPERTY()
	bool Clicked_X = false;
	UPROPERTY()
	bool Clicked_Y = false;
	UPROPERTY()
	bool Clicked_Z = false;
	UPROPERTY()
	bool Clicked_B = false;
	
	// 오버랩 됬을 때 중복 색상 변경
	UPROPERTY()
	bool Clicking_TX = false;
	UPROPERTY()
	bool Clicking_TY = false;
	UPROPERTY()
	bool Clicking_TZ = false;


	// SpawnActor 이전 위치 저장

	
	UFUNCTION()
	void Return_Previous_location();
	UFUNCTION()
	void AddPreviousLocation(const FVector& newLocation);
	
	std::stack<FVector> PreviousLocations;
	const int MaxLocations = 5;

	UPROPERTY()
	AJSH_Editor_SpawnActor* Previous_Click_Actor;

	UPROPERTY(Replicated)
	AJSH_Editor_SpawnActor* Now_Click_Actor;
	
#pragma endregion



#pragma region MainPlatform

	// 메인 플랫폼 일떄 기능 막아두는 bool 값
	UPROPERTY(Replicated)
	bool Bool_MainLock = true;

	// UPROPERTY(Replicated)
	// FString currgamemodename;
	
#pragma endregion


#pragma region Camera Control

	// 카메라 줌
	UPROPERTY(Replicated)
	bool Bool_ZoomMode = false;
	
	UFUNCTION()
	void Camera_Zoom_In();
	UFUNCTION()
	void Camera_Zoom_Out();
	UFUNCTION()
	void Camera_Zoom_Default();

	UPROPERTY()
	float ZoomFOV = 0;
	UPROPERTY()
	float ZoomSpeed = 0.3f; 
	UPROPERTY()
	float MinFOV = 120.0f;
	UPROPERTY()
	float MaxFOV = 10.0f;
	UPROPERTY()
	bool WheelOn = false;
	UPROPERTY()
	bool Bool_RightZoom = false;


	// 카메라 회전
	UFUNCTION()
	void CameraRight();
	UFUNCTION()
	void CameraLeft();
	UFUNCTION()
	void CameraDefault();
	UFUNCTION()
	void CameraReset();
	UPROPERTY()
	FRotator DefaultCameraleaning = FRotator(0, 0, 0);
	UPROPERTY()
	FRotator NewCameraRotation;
	UPROPERTY()
	float RotateSpeed = 0.2f;
	UPROPERTY()
	float CurrentAngl = 0.0f;


	// 마우스 감도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float MouseSensitivityYaw = 0.7f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float MouseSensitivityPitch = 0.7f;

	UFUNCTION()
	void Mouse_Sensitivity(const FInputActionValue& Value);
#pragma endregion


#pragma region Session
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UFirebaseComponent* FirebaseComponent;

	void Esc();
#pragma endregion

	UFUNCTION()
	void UI_Off();
	
};

