// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BetaPl/JSH_TheaterSpawnActor.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "JSH_BetaPlayer.generated.h"


class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;


UCLASS(config=Game)
class METASTUDIOS_API AJSH_BetaPlayer : public ACharacter
{
	GENERATED_BODY()


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_Grab;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_SpawnPop;

public:
	// Sets default values for this character's properties
	AJSH_BetaPlayer();


protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);


protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void BeginPlay();


public:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Tick(float DeltaTime) override;

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }



	UPROPERTY(Replicated)
	TArray<AActor*> PopList;

	UFUNCTION()
	void Grab();
	UFUNCTION(Server, Reliable)
	void Server_Grab(bool gtrue);
	UFUNCTION(NetMulticast, Reliable)
	void NetMulti_Grab(bool gtrue);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
	bool BHasPop;

	UPROPERTY(EditDefaultsOnly , BlueprintReadWrite, Replicated)
	bool PopOn;


	// 잡았을 때 위치
	UPROPERTY(EditDefaultsOnly , Category = Pistol, Replicated)
	class USceneComponent* HandComp;


	// 소유한 총을 기억.
	UPROPERTY(Replicated)
	class AActor* GrabPopActor;


	UPROPERTY(EditDefaultsOnly, Replicated)
	float GrabDistance = 150;

	UFUNCTION()
	void AttachPop(AActor* PopActor);
	UFUNCTION(Server, Reliable)
	void Server_AttachPop(AActor* PopActor);
	UFUNCTION(NetMulticast, Reliable)
	void NetMulti_AttachPop(AActor* PopActor);

	UFUNCTION()
	void DetachPop(AActor* PopActor);
	UFUNCTION(Server, Reliable)
	void Server_DetachPop(UCapsuleComponent* cap);
	UFUNCTION(NetMulticast, Reliable)
	void NetMulti_DetachPop(UCapsuleComponent* cap);


	UFUNCTION()
	void MyTakePop();
	UFUNCTION(Server, Reliable)
	void Server_MyTakePop();
	UFUNCTION(NetMulticast, Reliable)
	void NetMulti_MyTakePop();



	UFUNCTION()
	void MyReleasePop();
	UFUNCTION(Server, Reliable)
	void Server_MyReleasePop();
	UFUNCTION(NetMulticast, Reliable)
	void NetMulti_MyReleasePop();



	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	bool PopModeON = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	bool PopAnimationplay = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AxAnimations", Replicated)
	UAnimMontage* PopMontage;

	// UFUNCTION(Server, Reliable)
	// void Server_LeftMouseAction();
	//
	// UFUNCTION(NetMulticast, Reliable)
	// void NetMulti_LeftMouseAction();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	UCapsuleComponent* CapsuleComp;


	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	bool PopGrab_O = false;


	UPROPERTY(Replicated)
	AJSH_TheaterSpawnActor* OriginPop;
	
	UFUNCTION()
	void Spawn_Pop();
	
	UFUNCTION(Reliable, Server)
	void Server_Spawn_Pop(FVector slo, FRotator sro);
	
	UFUNCTION(Reliable, NetMulticast)
	void NetMulit_Pop(FVector slo, FRotator sro);
	
	UPROPERTY(Replicated, EditDefaultsOnly, Category = "Gizmo")
	TSubclassOf<AActor> PopClass;
	UPROPERTY(Replicated)
	AActor* PopAct = nullptr;
	UPROPERTY(Replicated)
	AJSH_TheaterSpawnActor* SpawnPop;

	FActorSpawnParameters SpawnParams;



	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UFirebaseComponent* FirebaseComponent;

	UPROPERTY()
	class UMainGameInstance* CHJ_Instance;
	
	void Esc();



	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite);
	bool Start_Movie_On = false;
};


