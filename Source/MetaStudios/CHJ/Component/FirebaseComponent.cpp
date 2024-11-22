// Fill out your copyright notice in the Description page of Project Settings.


#include "../CHJ/Component/FirebaseComponent.h"

#include "HttpModule.h"
#include "MediaPlayer.h"
#include "StreamMediaSource.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Net/UnrealNetwork.h"

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
    //AnonymousLogin();
}


// Called every frame
void UFirebaseComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UFirebaseComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UFirebaseComponent, MyMediaPlayer);
    DOREPLIFETIME(UFirebaseComponent, MyStreamMediaSource);
}

#pragma region firebase에 익명 로그인 한다.
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
            FString IDToken = JsonObject->GetStringField(TEXT("idToken"));
            
            // 이 ID 토큰을 저장하고 이후 Firebase Storage 요청에 사용합니다.
            AnonymousID = IDToken;
        }
    }
    else
    {
        // 로그인 실패 처리
    }
}
#pragma endregion

#pragma region 파이어베이스에 업로드 한다.
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
   HttpRequest->OnProcessRequestComplete().BindLambda([this, FileName](FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful)
      {
         if ( bWasSuccessful && Response->GetResponseCode() == 200 )
         {
                UE_LOG(LogTemp, Log, TEXT("File uploaded successfully"));
                GetUploadedFileUrl(FileName);
         }
         else
         {
            UE_LOG(LogTemp , Error , TEXT("File upload failed !!!"));
         }
      });

   // 요청 전송
   HttpRequest->ProcessRequest();
}
#pragma endregion

#pragma region 파이어베이스에서 영상 다운로드 한다
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
#pragma endregion 

#pragma region 업로드 한 파일 이름의 url을 가져온다.
void UFirebaseComponent::GetUploadedFileUrl(const FString& FileName)
{
    // HTTP 요청 생성 (메타데이터 요청)
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
    HttpRequest->SetURL(FirebaseStorageUrl + FileName + "?alt=media&token=" + AnonymousID); // 필요한 경우 토큰 추가
    HttpRequest->SetVerb("GET");
    HttpRequest->SetHeader("Authorization", "Bearer " + AnonymousID); // 익명 접근의 경우 필요하지 않을 수 있음

    // HTTP 응답 처리
    HttpRequest->OnProcessRequestComplete().BindLambda([this](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
    {
        if (bWasSuccessful && Response->GetResponseCode() == 200)
        {
            FString FileUrl = Response->GetURL();
            UE_LOG(LogTemp, Warning, TEXT("File URL: %s"), *FileUrl);
            //MyStreamMediaSource->StreamUrl = FileUrl;
            //MyMediaPlayer->OpenSource(MyStreamMediaSource);
            MultiRPC_GetStreamUrl(FileUrl);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to get file URL"));
        }
    });

    // 요청 전송
    HttpRequest->ProcessRequest();
}
#pragma endregion

#pragma region 영상 URL 받고 멀티로 뿌린다
void UFirebaseComponent::MultiRPC_GetStreamUrl_Implementation(const FString& FileName)
{
    MyStreamMediaSource->StreamUrl = FileName;
    MyMediaPlayer->OpenSource(MyStreamMediaSource);
    
}
#pragma endregion


#pragma region 파일 이름만 가져온다.
FString UFirebaseComponent::GetLine(const FString& str)
{
    TArray<FString> paths;
    str.ParseIntoArray(paths, TEXT("/"));

    TArray<FString> receive;
    
    paths[paths.Num() - 1].ParseIntoArray(receive, TEXT("."));

    return receive[0];
}
#pragma endregion


