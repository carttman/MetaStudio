// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/InputActionValue.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/InputMappingContext.h"
#include "SpaceshipCharacter.generated.h"

UCLASS()
class METASTUDIOS_API ASpaceshipCharacter : public ACharacter
{
	GENERATED_BODY()


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_SpaceshipMove;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_SpaceshipLook;

public:
	// Sets default values for this character's properties
	ASpaceshipCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//UPROPERTY(EditDefaultsOnly)
	//class UInputAction* IA_SpaceshipMove;

	void OnMyActionMove(const FInputActionValue& Value);

	FVector Direction;
	
	// 마우스 입력을 받아서 회전값을 처리하고싶다.
// 	UPROPERTY(EditDefaultsOnly)
// 	class UInputAction* IA_SpaceshipLook;

	void OnMyActionLook(const FInputActionValue& Value);

private:
	UPROPERTY(VisibleAnywhere)
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere)
	class UCameraComponent* CameraComp;
};
