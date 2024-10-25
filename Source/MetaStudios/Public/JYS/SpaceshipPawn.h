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


private:
	
	FVector MovementDirection;

	FVector direction;

	UPROPERTY(EditAnywhere)
	float MovementSpeed = 500.0f;

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* SpaceshipMesh;

	UPROPERTY(VisibleAnywhere)
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere)
	class UCameraComponent* CameraComp;

};
