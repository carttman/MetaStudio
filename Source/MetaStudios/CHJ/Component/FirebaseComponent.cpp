// Fill out your copyright notice in the Description page of Project Settings.


#include "../CHJ/Component/FirebaseComponent.h"

#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "MetaStudios/JSH/JSH_Player.h"

// Sets default values for this component's properties
UFirebaseComponent::UFirebaseComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UFirebaseComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UFirebaseComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UFirebaseComponent::AnonymousLogin()
{
    FString FirebaseAPIKey = "AIzaSyCIixsAehUcAmotkhFkHWa8kXbeO3F8A-I";
    FString URL = "https://identitytoolkit.googleapis.com/v1/accounts:signUp?key=" + FirebaseAPIKey;

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(URL);
    Request->SetVerb("POST");
    Request->SetHeader("Content-Type", "application/json");

    FString RequestBody = "{\"returnSecureToken\":true}";
    Request->SetContentAsString(RequestBody);

    Request->OnProcessRequestComplete().BindUObject(this, &UFirebaseComponent::OnAnonymousLoginComplete);
    Request->ProcessRequest();
}

void UFirebaseComponent::OnAnonymousLoginComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (bWasSuccessful && Response->GetResponseCode() == 200)
    {
        FString JsonResponse = Response->GetContentAsString();
        TSharedPtr<FJsonObject> JsonObject;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonResponse);

        if (FJsonSerializer::Deserialize(Reader, JsonObject))
        {
            FString IDToken = JsonObject->GetStringField("idToken");
            
            // 이 ID 토큰을 저장하고 이후 Firebase Storage 요청에 사용합니다.
            AnonymousID = IDToken;
        }
    }
    else
    {
        // 로그인 실패 처리
    }
}

void UFirebaseComponent::FileUploadToFirebase(const FString& FilePath, const FString& FileName)
{

   // 파일 읽기
   TArray<uint8> FileData;
   if ( !FFileHelper::LoadFileToArray(FileData , *FilePath) )
   {
      UE_LOG(LogTemp , Error , TEXT("파일 읽기 실패!: %s") , *FilePath);
      return;
   }
    UE_LOG(LogTemp, Log, TEXT("File : %s"), *FilePath);
   // HTTP 요청 생성
   TSharedRef<IHttpRequest , ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
   HttpRequest->SetURL(FirebaseStorageUrl + FileName + "?alt=media");
   HttpRequest->SetVerb("POST");
   HttpRequest->SetHeader("Content-Type" , "application/octet-stream");
   HttpRequest->SetHeader("Authorization" , "Bearer " + AnonymousID); // 익명 접근의 경우 필요하지 않을 수 있음
   HttpRequest->SetContent(FileData);

   // HTTP 응답 처리
   HttpRequest->OnProcessRequestComplete().BindLambda([this](FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful)
      {
         if ( bWasSuccessful && Response->GetResponseCode() == 200 )
         {
                UE_LOG(LogTemp, Log, TEXT("File uploaded successfully"));
               // 이긴 쪽의 클라이언트에서 업로드가 완료되면 서버의 다운로드 함수를 부르기
         }
         else
         {
            UE_LOG(LogTemp , Error , TEXT("File upload failed !!!"));
         }
      });

   // 요청 전송
   HttpRequest->ProcessRequest();
}

void UFirebaseComponent::FileDownloadFromFirebase(const FString& SavePath, const FString& FileName)
{
    // HTTP 요청 생성
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
    HttpRequest->SetURL(FirebaseStorageUrl + FileName + "?alt=media");
    HttpRequest->SetVerb("GET");

    // HTTP 응답 처리
    HttpRequest->OnProcessRequestComplete().BindLambda([SavePath, FileName, this](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
        {
            if (bWasSuccessful && Response->GetResponseCode() == 200)
            {
                // 파일 저장
                if (FFileHelper::SaveArrayToFile(Response->GetContent(), *SavePath))
                {
                    UE_LOG(LogTemp, Log, TEXT("File downloaded successfully"));
                    AJSH_Player* You = Cast<AJSH_Player>(Me->GetWorld()->GetFirstPlayerController()->GetCharacter());
                }
                else
                {
                    UE_LOG(LogTemp, Error, TEXT("Failed to save file: %s"), *SavePath);
                }
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("File download failed!!"));
            }
        });

    // 요청 전송
    HttpRequest->ProcessRequest();
}