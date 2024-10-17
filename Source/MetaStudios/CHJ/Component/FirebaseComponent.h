// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/IHttpRequest.h"
#include "FirebaseComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class METASTUDIOS_API UFirebaseComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFirebaseComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void AnonymousLogin();
	UFUNCTION()
	void FileUploadToFirebase(const FString& FilePath, const FString& FileName);
	UFUNCTION()
	void FileDownloadFromFirebase(const FString& SavePath, const FString& FileName);
	UPROPERTY()
	class AJSH_Player* Me;

private:
   
	FString AnonymousID = "";

	FString FirebaseStorageUrl = "https://firebasestorage.googleapis.com/v0/b/wavupdownload.appspot.com/o/";
	void OnAnonymousLoginComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

};
