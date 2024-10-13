

#include "JSH_OBSWebSocket.h"
#include "WebSocketsModule.h"
#include "Misc/SecureHash.h" 
#include "JsonUtilities.h"



void UJSH_OBSWebSocket::Init()
{
    Super::Init();

    // OBSWebSocketConnect();
    

    // 녹화 종료 버튼을 누리지 않고, 게임을 껐을떄도 정상적으로 녹화가 종료 되도록 (1) =====
    APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
    if (PlayerController)
    {
        // Get the controlled pawn and cast it to AJSH_Player
        DirectorPlayer = Cast<AJSH_Player>(PlayerController->GetPawn());
    }
    // ============================================================================

    
}

void UJSH_OBSWebSocket::Shutdown()
{

    // OBSWebSocketDisConnect();

    // 녹화 종료 버튼을 누리지 않고, 게임을 껐을떄도 정상적으로 녹화가 종료 되도록 (2) =====
    // if (DirectorPlayer != nullptr && DirectorPlayer->Recording == true)
    // {
    //     DirectorPlayer->StopRecording();
    // }
    // ============================================================================
    
    Super::Shutdown();
}


// WebSocket 연결
void UJSH_OBSWebSocket::OBSWebSocketConnect()
{
    // if (!FModuleManager::Get().IsModuleLoaded("WebSockets"))
    // {
    //     FModuleManager::Get().LoadModuleChecked("WebSockets");
    // }
    //
    // WebSocket = FWebSocketsModule::Get().CreateWebSocket("ws://192.168.0.4:4455");
    //
    //
    //
    // WebSocket->OnConnected().AddLambda([]()
    // {
    //     GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, "Success");
    // });
    //
    // WebSocket->OnConnectionError().AddLambda([](const FString& Error)
    // {
    //     GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Error");
    // });
    //
    // WebSocket->OnClosed().AddLambda([](int32 StatusCode, const FString& Reason, bool bWasClean)
    // {
    //     GEngine->AddOnScreenDebugMessage(-1, 15.0f, bWasClean ? FColor::Green : FColor::Red, "Closed" + Reason);
    // });
    //
    //
    // WebSocket->Connect();
}

void UJSH_OBSWebSocket::OBSWebSocketDisConnect()
{
    // if (WebSocket->IsConnected())
    // {
    //     WebSocket->Close();
    // }

    
}

void UJSH_OBSWebSocket::StartRecord()
{
    // if (WebSocket.IsValid() && WebSocket->IsConnected())
    // {
    //     // StartRecording 명령을 OBS에 전송
    //     FString StartRecordMessage = TEXT("{\"request-type\": \"StartRecording\", \"message-id\": \"1\"}");
    //     
    //     WebSocket->Send(StartRecordMessage);
    // }
    // else
    // {
    //     UE_LOG(LogTemp, Warning, TEXT("WebSocket is not connected!"));
    // }
    
    // if (WebSocket.IsValid() && WebSocket->IsConnected())
    // {
    //     // JSON 객체 생성
    //     TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
    //
    //     // 필요한 명령어와 파라미터 추가
    //     JsonObject->SetStringField("op", "6"); // Request Type
    //     JsonObject->SetStringField("d", "StartRecord");
    //
    //     // JSON 형식으로 변환
    //     FString OutputString;
    //     TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
    //     FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
    //
    //     // WebSocket을 통해 메시지 전송
    //     WebSocket->Send(OutputString);
    // }
    // else
    // {
    //     UE_LOG(LogTemp, Warning, TEXT("WebSocket is not connected!"));
    // }
}

