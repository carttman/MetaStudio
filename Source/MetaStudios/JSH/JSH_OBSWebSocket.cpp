

#include "JSH_OBSWebSocket.h"
#include "WebSocketsModule.h"
#include "Misc/SecureHash.h" 
#include "JsonUtilities.h"



void UJSH_OBSWebSocket::Init()
{
    Super::Init();
    
}

void UJSH_OBSWebSocket::Shutdown()
{
	// 녹화 종료 버튼을 누리지 않고, 게임을 껐을떄도 정상적으로 녹화가 종료 되도록
	if (Recording)
	{
		StartRecord();
	}
	
	Super::Shutdown();
}





void UJSH_OBSWebSocket::StartRecord()
{
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


			// // 환경 변수 설정
			//
			// FString CMDPath = TEXT("C:/Windows/System32"); // 사용자가 지정한 경로
			// FString SettingPath = FString::Printf(TEXT("setx PATH \"%PATH%;%sffmpeg/bin\""), *AutoPath);
			
			
			// @@ Suhyeok_Home @@
			// FString FilePath = FString::Printf(TEXT("%sffmpeg/UE_%s.mkv"), *AutoPath, *DateTimeString);
			// FString ExecutablePath = FString::Printf(TEXT("%sffmpeg/bin/ffmpeg.exe"), *AutoPath);   // Path to ffmpeg.exe
			// FString Params = FString::Printf(TEXT("-f dshow -i audio=\"Line 1(Virtual Audio Cable)\" -f gdigrab -offset_x 0 -offset_y 0 -video_size 1920x1080 -framerate 30 -draw_mouse 1 -probesize 1000M -i desktop -c:v h264_nvenc -qp 0 %s"), *FilePath);


			
			// MSI
			FString FilePath = FString::Printf(TEXT("%sffmpeg/UE_%s.mkv"), *AutoPath, *DateTimeString);
			FString ExecutablePath = FString::Printf(TEXT("%sffmpeg/bin/ffmpeg.exe"), *AutoPath);   // Path to ffmpeg.exe
			FString Params = FString::Printf(TEXT("-f dshow -i audio=\"마이크 배열(Realtek(R) Audio)\" -f gdigrab -offset_x 0 -offset_y 0 -video_size 2560x1600 -framerate 30 -draw_mouse 1 -probesize 1000M -i desktop -c:v h264_nvenc -qp 0 %s"), *FilePath);

			//asusx
			//FString FilePath = FString::Printf(TEXT("%sffmpeg/UE_%s.mkv"), *AutoPath, *DateTimeString);
			//FString ExecutablePath = FString::Printf(TEXT("%sffmpeg/bin/ffmpeg.exe"), *AutoPath);   // Path to ffmpeg.exe
			//FString Params = FString::Printf(TEXT("-f dshow -i audio=\"마이크 배열(Realtek(R) Audio)\" -f gdigrab -offset_x 0 -offset_y 0 -video_size 2560x1600 -framerate 30 -draw_mouse 1 -probesize 1000M -i desktop -c:v h264_nvenc -qp 0 %s"), *FilePath);

			
			
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

