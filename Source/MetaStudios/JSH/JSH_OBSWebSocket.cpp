

#include "JSH_OBSWebSocket.h"
#include "WebSocketsModule.h"


void UJSH_OBSWebSocket::Init()
{
    Super::Init();

    if (!FModuleManager::Get().IsModuleLoaded("WebSockets"))
    {
        FModuleManager::Get().LoadModuleChecked("WebSockets");
    }

    WebSocket = FWebSocketsModule::Get().CreateWebSocket("ws://192.168.0.4:4455");

    WebSocket->Connect();
    
    // ConnectToOBS();  // 게임 시작 시 WebSocket 연결
}

void UJSH_OBSWebSocket::Shutdown()
{
    if (WebSocket->IsConnected())
    {
        WebSocket->Close();
    }
    
    Super::Shutdown();



    
    // if (OBSWebSocket.IsValid())
    // {
    //     OBSWebSocket->Close();  // 게임이 종료될 때 WebSocket 연결 해제
    // }
}



// void UJSH_OBSWebSocket::ConnectToOBS()
// {
//     if (!FModuleManager::Get().IsModuleLoaded("WebSockets"))
//     {
//         FModuleManager::Get().LoadModuleChecked("WebSockets");
//     }
//
//     FString ServerURL = "ws://192.168.0.4:4455";  // 서버 IP와 포트
//
//     // WebSocket 초기화
//     OBSWebSocket = FWebSocketsModule::Get().CreateWebSocket(ServerURL);
//
//     OBSWebSocket->OnConnected().AddLambda([this]()
//     {
//         UE_LOG(LogTemp, Log, TEXT("Connected to OBS WebSocket"));
//
//         // 비밀번호 인증 메시지 전송
//         FString AuthMessage = FString::Printf(TEXT("{\"op\": 1, \"d\": {\"rpcVersion\": 1, \"authentication\": \"%s\"}}"), *FString("z8cJSfpNLoVzvacB"));
//         OBSWebSocket->Send(AuthMessage);
//     });
//
//     OBSWebSocket->OnConnectionError().AddLambda([](const FString& Error)
//     {
//         UE_LOG(LogTemp, Error, TEXT("WebSocket Connection Error: %s"), *Error);
//     });
//
//     OBSWebSocket->Connect();
// }

// void UJSH_OBSWebSocket::StartRecording()
// {
//     if (OBSWebSocket.IsValid() && OBSWebSocket->IsConnected())
//     {
//         FString StartRecordingMessage = TEXT("{\"op\": 6, \"d\": {\"requestType\": \"StartRecording\", \"requestId\": \"startRecording\"}}");
//         OBSWebSocket->Send(StartRecordingMessage);
//         UE_LOG(LogTemp, Log, TEXT("Sent StartRecording Command to OBS"));
//     }
// }
//
// void UJSH_OBSWebSocket::StopRecording()
// {
//     if (OBSWebSocket.IsValid() && OBSWebSocket->IsConnected())
//     {
//         FString StopRecordingMessage = TEXT("{\"op\": 6, \"d\": {\"requestType\": \"StopRecording\", \"requestId\": \"stopRecording\"}}");
//         OBSWebSocket->Send(StopRecordingMessage);
//         UE_LOG(LogTemp, Log, TEXT("Sent StopRecording Command to OBS"));
//     }
// }
