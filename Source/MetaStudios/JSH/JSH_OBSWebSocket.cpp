

#include "JSH_OBSWebSocket.h"
#include "WebSocketsModule.h"
#include "Misc/SecureHash.h" 
#include "JsonUtilities.h"



void UJSH_OBSWebSocket::Init()
{
    Super::Init();

    // OBSWebSocketConnect();
    

    // 녹화 종료 버튼을 누리지 않고, 게임을 껐을떄도 정상적으로 녹화가 종료 되도록 (1) =====
    // APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
    // if (PlayerController)
    // {
    //     // Get the controlled pawn and cast it to AJSH_Player
    //     DirectorPlayer = Cast<AJSH_Player>(PlayerController->GetPawn());
    // }
    // ============================================================================

    
}

void UJSH_OBSWebSocket::Shutdown()
{
	if (Recording)
	{
		StartRecord();
	}
	
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
	
	// 테스트 할때 문제 없게, 일단 서버만 허용


		if (!Recording)
		{
			// 녹화 시작 코드 
			// FString Command = TEXT("ffmpeg -f dshow -i audio=\"Line 1(Virtual Audio Cable)\" -f gdigrab -offset_x 0 -offset_y 0 -video_size 1920x1080 -framerate 30 -draw_mouse 1 -probesize 1000M -i desktop -c:v h264_nvenc -qp 0 C:\\Users\\jsh\\Documents\\GitHub\\MetaStudio\\ffmpeg-7.1-full_build\\UE_Recording_Screen_with_Audio.mkv");
			//
			//
			// FString ExecutablePath = TEXT("C:/Users/jsh/Documents/GitHub/MetaStudio/ffmpeg-7.1-full_build/bin/ffmpeg.exe");  // Path ffmpeg.exe
			// FString Params = TEXT("-f dshow -i audio=\"Line 1(Virtual Audio Cable)\" -f gdigrab -offset_x 0 -offset_y 0 -video_size 1920x1080 -framerate 30 -draw_mouse 1 -probesize 1000M -i desktop -c:v h264_nvenc -qp 0 C:\\Users\\jsh\\Documents\\GitHub\\MetaStudio\\ffmpeg-7.1-full_build\\UE_Recording_Screen_with_Audio.mkv");


			
			// 기본 세팅을 위한 값들
			FDateTime Now = FDateTime::Now();
			FString DateTimeString = Now.ToString(TEXT("%Y-%m-%d_%H-%M-%S")); 
			FString AutoPath = FPaths::ProjectUserDir();


			// 환경 변수 설정

			FString CMDPath = TEXT("C:/Windows/System32"); // 사용자가 지정한 경로
			FString SettingPath = FString::Printf(TEXT("setx PATH \"%PATH%;%sffmpeg/bin\""), *AutoPath);

			// int32 ReturnCode;
			// FString Output, Error;
			// FPlatformProcess::ExecProcess(*CMDPath, *SettingPath, &ReturnCode, &Output, &Error);


			
			
			// @@ Suhyeok_Home @@
			// FString FilePath = FString::Printf(TEXT("C:\\Users\\jsh\\Documents\\GitHub\\MetaStudio\\ffmpeg-7.1-full_build\\UE_Recording_Screen_with_Audio_%s.mkv"), *DateTimeString);
			// FString ExecutablePath = TEXT("C:/Users/jsh/Documents/GitHub/MetaStudio/ffmpeg-7.1-full_build/bin/ffmpeg.exe");  // Path to ffmpeg.exe
			// FString Params = FString::Printf(TEXT("-f dshow -i audio=\"Line 1(Virtual Audio Cable)\" -f gdigrab -offset_x 0 -offset_y 0 -video_size 1920x1080 -framerate 30 -draw_mouse 1 -probesize 1000M -i desktop -c:v h264_nvenc -qp 0 %s"), *FilePath);


			// // 언리얼 경로 함수 (Saved 폴더)
			// FString FilePathTest = FPaths::ProjectSavedDir();
			//
			// // @@ Suhyeok_Msi @@
			// FString FilePath = FString::Printf(TEXT("%s\\ffmpeg-7.1-full_build\\UE_Recording_Screen_with_Audio_%s.mkv"), *FilePathTest, *DateTimeString);
			// FString ExecutablePath = FString::Printf(TEXT("%s\\ffmpeg-7.1-full_build\\bin\\ffmpeg.exe"), *FilePathTest);   // Path to ffmpeg.exe
			// FString Params = FString::Printf(TEXT("-f dshow -i audio=\"Microphone Array(Intel® Smart Sound Technology for Digital Microphones)\" -f gdigrab -offset_x 0 -offset_y 0 -video_size 2560x1600 -framerate 30 -draw_mouse 1 -probesize 1000M -i desktop -c:v h264_nvenc -qp 0 %s"), *FilePath);
			
			
			FString FilePath = FString::Printf(TEXT("%sffmpeg/UE_%s.mkv"), *AutoPath, *DateTimeString);
			FString ExecutablePath = FString::Printf(TEXT("%sffmpeg/bin/ffmpeg.exe"), *AutoPath);   // Path to ffmpeg.exe
			FString Params = FString::Printf(TEXT("-f dshow -i audio=\"Microphone Array(Intel® Smart Sound Technology for Digital Microphones)\" -f gdigrab -offset_x 0 -offset_y 0 -video_size 2560x1600 -framerate 30 -draw_mouse 1 -probesize 1000M -i desktop -c:v h264_nvenc -qp 0 %s"), *FilePath);

			
			
			FProcHandle ProcessHandle = FPlatformProcess::CreateProc(
				*ExecutablePath, // Executable path
				*Params,         // Command parameters
				true,            // bLaunchDetached (true to run in the background)
				false,           // bLaunchHidden (false if you want to see the console window)
				false,           // bLaunchReallyHidden (same as above)
				nullptr,         // OutProcessID
				0,               // Priority
				nullptr,         // Optional working directory
				nullptr          // Pipe write to nullptr (not needed)
			);

			PH = ProcessHandle;
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, "Start Recording");
			
			if (ProcessHandle.IsValid())
			{
				UE_LOG(LogTemp, Log, TEXT("Recording started successfully in the background"));
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to start recording process."));
			}
		}
		else
		{
			if (PH.IsValid())
			{
				
				FPlatformProcess::TerminateProc(PH);
				FPlatformProcess::CloseProc(PH);  
	        
				UE_LOG(LogTemp, Log, TEXT("Recording stopped and FFmpeg process terminated"));
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to stop recording. No valid process handle found."));
			}
			GEngine->AddOnScreenDebugMessage(-2, 10.0f, FColor::Yellow, "End Recording");
		}
		Recording = !Recording;
}

