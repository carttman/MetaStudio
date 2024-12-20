#include "JSH_Editor_SpawnActor.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Components/PrimitiveComponent.h"
#include "DrawDebugHelpers.h"
#include "JSH_PlayerController.h"
#include "Engine/EngineTypes.h"
#include "Net/UnrealNetwork.h"

// Constructor: Set default values
AJSH_Editor_SpawnActor::AJSH_Editor_SpawnActor()
{
    PrimaryActorTick.bCanEverTick = true; 

    RootScence = CreateDefaultSubobject<USceneComponent>(TEXT("RootScence"));
    RootComponent = RootScence;
    
    AssetMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body"));
    AssetMesh->SetupAttachment(RootComponent);
    AssetMesh->SetRenderCustomDepth(false);

    
    // ConstructorHelpers::FObjectFinder<UStaticMesh> TMesh(TEXT("/Script/Engine.StaticMesh'/Game/JSH/Asset/Teenieping/chachaping.chachaping'"));
    // if (TMesh.Succeeded())
    // {
    //     AssetMesh->SetStaticMesh(TMesh.Object);
    // }


    //AssetMesh->OnClicked.AddDynamic(this, &AJSH_Editor_SpawnActor::OnMeshClicked);


    // 기즈모 스폰을 위한
    static ConstructorHelpers::FClassFinder<AActor> GizmoBPClass(TEXT("/Game/JSH/BP/Gizmo/BP_Gizmo.BP_Gizmo_C"));
    if (GizmoBPClass.Succeeded())
    {
        GizmoClass = GizmoBPClass.Class;
    }
}

// Called when the game starts
void AJSH_Editor_SpawnActor::BeginPlay()
{
    Super::BeginPlay();

    UGameplayStatics::GetAllActorsWithTag(GetWorld(), TEXT("AJSH_Player"), OriginPlayer_Box);
    if(OriginPlayer_Box.Num() > 0)
    {
        OriginPlayer = Cast<AJSH_Player>(OriginPlayer_Box[0]);

        // 플레이어 컨트롤러
        JPlayerController = Cast<AJSH_PlayerController>(OriginPlayer->GetController());
        if (JPlayerController)
        {
            JPlayerController->bEnableTouchEvents = false;
            // OriginPlayer = Cast<AJSH_Player>(JPlayerController->GetPawn());
        }
    }



    
    // // 플레이어 컨트롤러
    // JPlayerController = Cast<AJSH_PlayerController>(GetWorld()->GetFirstPlayerController());
    // if (JPlayerController)
    // {
    //     JPlayerController->bEnableTouchEvents = false;
    //    // OriginPlayer = Cast<AJSH_Player>(JPlayerController->GetPawn());
    // }

}

// Called every frame
void AJSH_Editor_SpawnActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);


    // 클릭 되었을 때에 아웃라인 생기도록 + 안 쪽 기즈모 클릭을 위한 노콜리젼 처리
    if (OriginPlayer)
    {
        if (OriginPlayer->Editor_SpawnActor == this)
        {
            AssetMesh->SetRenderCustomDepth(true);
            
            // 기즈모 트래킹 되는지 확인 필요
            //AssetMesh->SetCollisionProfileName(TEXT("NoCollision"));
        }
        else
           {
            AssetMesh->SetRenderCustomDepth(false);

            // 기즈모 트래킹 되는지 확인 필요
            //AssetMesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));
            
            // if (GizmoActor && IsValid(GizmoActor) && GizmoActor->GetWorld())
            // {
            //     GizmoActor->Destroy();
            //     // GizmoActor->SetActorHiddenInGame(false);
            // }
        }  
    }
}

void AJSH_Editor_SpawnActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);


    DOREPLIFETIME(AJSH_Editor_SpawnActor, OriginPlayer);
    DOREPLIFETIME(AJSH_Editor_SpawnActor, OriginPlayer_Box);
    DOREPLIFETIME(AJSH_Editor_SpawnActor, JPlayerController);
    DOREPLIFETIME(AJSH_Editor_SpawnActor, GizmoClass);
    DOREPLIFETIME(AJSH_Editor_SpawnActor, ThisTransform);
    DOREPLIFETIME(AJSH_Editor_SpawnActor, GizmoActor);
    DOREPLIFETIME(AJSH_Editor_SpawnActor, OriginGizmo);
    DOREPLIFETIME(AJSH_Editor_SpawnActor, GizmoX_ON);
    DOREPLIFETIME(AJSH_Editor_SpawnActor, GizmoY_ON);
    DOREPLIFETIME(AJSH_Editor_SpawnActor, GizmoZ_ON);
    DOREPLIFETIME(AJSH_Editor_SpawnActor, GizmoB_ON);
}



void AJSH_Editor_SpawnActor::Get_PlayerController()
{
    Server_Get_PlayerController();
}

void AJSH_Editor_SpawnActor::Server_Get_PlayerController_Implementation()
{
    NetMulti_Get_PlayerController();
}
void AJSH_Editor_SpawnActor::NetMulti_Get_PlayerController_Implementation()
{
    // 플레이어 컨트롤러
    JPlayerController = Cast<AJSH_PlayerController>(GetWorld()->GetFirstPlayerController());
    if (JPlayerController)
    {
        JPlayerController->bEnableTouchEvents = false;
        // OriginPlayer = Cast<AJSH_Player>(JPlayerController->GetPawn());
    }
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), TEXT("AJSH_Player"), OriginPlayer_Box);
    if(OriginPlayer_Box.Num() > 0)
    {
        OriginPlayer = Cast<AJSH_Player>(OriginPlayer_Box[0]);
    }
}




void AJSH_Editor_SpawnActor::NotifyActorOnClicked(FKey ButtonPressed)
{
    Super::NotifyActorOnClicked(ButtonPressed);

    if (!JPlayerController) return;
    
    //겹쳐져 있을 때에 기즈모가 클릭되고 스폰액터는 클릭 안 되도록
    if (OriginPlayer->Gizmo_Detecting) return;
    
    if (OriginPlayer->Editor_SpawnActor == this) return;

    Server_NotifyActorOnClicked();

    GizmoSpawn();
}


void AJSH_Editor_SpawnActor::Server_NotifyActorOnClicked_Implementation()
{
    NetMulti_NotifyActorOnClicked();
}


void AJSH_Editor_SpawnActor::NetMulti_NotifyActorOnClicked_Implementation()
{
    // 클릭 했을때 자신의 정보를 Player에 저장
    OriginPlayer->SaveEditorActor(this);

    
    // 이전 위치 돌아가는 함수 
    if (OriginPlayer->Now_Click_Actor == nullptr)
    {
        OriginPlayer->Now_Click_Actor = this;
    }
    else if (OriginPlayer->Now_Click_Actor != nullptr)
    {
        OriginPlayer->Previous_Click_Actor = OriginPlayer->Now_Click_Actor;
        OriginPlayer->Now_Click_Actor = this;
    }
    
    UE_LOG(LogTemp, Error, TEXT("Click"));
}





void AJSH_Editor_SpawnActor::Onclicked()
{
    AssetMesh->SetRenderCustomDepth(true);
    AssetMesh->SetCollisionProfileName(TEXT("NoCollision"));
}

void AJSH_Editor_SpawnActor::Unclicked()
{
    // AssetMesh->SetRenderCustomDepth(false);
    // AssetMesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));
    if (GizmoActor && IsValid(GizmoActor) && GizmoActor->GetWorld())
    {
        GizmoActor->Destroy();
        // GizmoActor->SetActorHiddenInGame(false);
    }
}


void AJSH_Editor_SpawnActor::GizmoSpawn()
{
    // ThisTransform: 현재 actor의 위치로 설정
    //FTransform ThisTransform2 = this->GetActorTransform();

    // 기즈모를 통해 actor의 크기를 조정하고, 다른 actor 선택 후 크기 조정했던 actor 재 선택 시 transform으로 spawn하니깐
    // 기즈모 크기도 같이 늘어나는 문제 때문에 , location이랑 rotaion 만으로 해야할 듯
    FVector ThisLocation = this->GetActorLocation();
    FRotator Thisrotator = this->GetActorRotation();

    // Spawn parameters 설정
    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    UE_LOG(LogTemp, Log, TEXT("Trying to load GizmoClass"));

    if (GizmoClass)
    {
        UE_LOG(LogTemp, Log, TEXT("GizmoClass 로드 성공"));
        
        // GizmoActor: 블루프린트를 스폰합니다.
        GizmoActor = GetWorld()->SpawnActor<AActor>(GizmoClass, ThisLocation, Thisrotator, SpawnParams);
        
        OriginGizmo = Cast<AJSH_Gizmo>(GizmoActor);
        // Gizmo 내부 컨트롤러 -> Gizmo 내부 Child Actor 찾기 -> 내부 child actor들 컨트롤러 로드 -> player에 정보 전달
        //OriginGizmo->BeginPlayerContorller(JPlayerController);
        
        OriginGizmo->Begin_PlayerData(OriginPlayer, JPlayerController);

        

        // OriginGizmo를 this에 첨부합니다.
        //OriginGizmo->AttachToComponent(RootComponent,  FAttachmentTransformRules::SnapToTargetNotIncludingScale);


        
        if (GizmoActor)
        {
            UE_LOG(LogTemp, Log, TEXT("GizmoActor 스폰 성공"));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("GizmoActor 스폰 실패"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("GizmoClass 로드 실패"));
    }
}

void AJSH_Editor_SpawnActor::Gizmo_Spawn()
{
    //     UE_LOG(LogTemp, Error, TEXT("pak 1"));
    // // 1. 현재 Actor 위치 가져오기
    // ThisTransform = this->GetActorTransform();
    //
    //
    // UE_LOG(LogTemp, Error, TEXT("pak 2"));
    //
    // // 2. 기즈모를 ThisTransform 위치에 스폰하기
    // FActorSpawnParameters SpawnParams;
    // SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	   //
    // AGameModeBase* tt = Cast<AGameModeBase>(GetWorld()->GetAuthGameMode());
	   //
    // UE_LOG(LogTemp, Error, TEXT("pak 3"));
    // // 3. 블루프린트 클래스 로드 -> 스폰
    // UClass* GizmoClass = StaticLoadClass(AActor::StaticClass(), nullptr, TEXT("/Game/JSH/BP/Gizmo/BP_Gizmo.BP_Gizmo_C"));
    //
    // if (GizmoClass)
    // {
    //     UE_LOG(LogTemp, Error, TEXT("pak 4"));
    //     GizmoActor = GetWorld()->SpawnActorDeferred<AActor>(GizmoClass, ThisTransform);
    //
    //     OriginGizmo = Cast<AJSH_Gizmo>(GizmoActor);
    //
    //     // Gizmo 내부 컨트롤러 -> Gizmo 내부 Child Actor 찾기 -> 내부 child actor들 컨트롤러 로드 -> player에 정보 전달
    //     OriginGizmo->BeginPlayerContorller(JPlayerController);
    //
    //     // Gizmo 내부 Child Actor 찾기
    //     //OriginGizmo->Child_Actor_Detect();
    //     // OriginGizmo->Origin_Translate_X->BeginPlayerContorller(JPlayerController);
    //     // OriginGizmo->Origin_Translate_Y->BeginPlayerContorller(JPlayerController);
    //     // OriginGizmo->Origin_Translate_Z->BeginPlayerContorller(JPlayerController);
    //     // OriginGizmo->Origin_Translate_Box->BeginPlayerContorller(JPlayerController);
    //     
    //     UE_LOG(LogTemp, Error, TEXT("pak 5"));
    //     // 원랜 켜야하는데 beginplay에서 해주고 있어서 일단 주석 처리
    //     //AJSH_PlayerController* PlayerController = Cast<AJSH_PlayerController>(GetWorld()->GetFirstPlayerController());
    //
    //     //AActor* Gizmo = Cast<APawn>(GizmoActor);
    //     if (GizmoActor)
    //     {
    //         UE_LOG(LogTemp, Error, TEXT("pak 6"));
    //         UE_LOG(LogTemp, Error, TEXT("Component 111111"));
    //         UGameplayStatics::FinishSpawningActor(GizmoActor, ThisTransform);
    //
    //         // // GizmoActor 스폰 논리
    //         if (GizmoActor != nullptr)
    //         {
    //             UE_LOG(LogTemp, Error, TEXT("pak 7"));
    //             //GizmoActor->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
    //            //GizmoActor->AttachToActor(this, FAttachmentTransformRules::SnapToTargetIncludingScale);
    //         }
    //     }
    // }
}



void AJSH_Editor_SpawnActor::DestroyThis()
{
    this->Destroy();
    if (GizmoActor)
    {
        GizmoActor->Destroy();
    }
}


void AJSH_Editor_SpawnActor::AddPreviousLocation(const FVector& newLocation)
{
    if (PreviousLocations.size() >= MaxLocations)
    {
        PreviousLocations.pop();  // 제일 오래된 위치 제거
    }
    PreviousLocations.push(newLocation);
}




void AJSH_Editor_SpawnActor::ReturnPreviousLocation()
{
    if (!OriginPlayer->EditorMode_B) return;
    if (OriginPlayer->DisableEdit_b) return;
	
    // 클릭 중에 q나 tap누르면 튕기는 오류 때문에 + 잡고 있을 떄 누르면 모드 바껴도 원래 상태로 
    if (OriginPlayer->Gizmo_Clicking_forError) return;

    // 여기는 UI 바뀌는거 넣음 될 듯
    if (OriginPlayer->Editor_SpawnActor == nullptr) return;
    ////////////////////////////////////////////////////////////////////////////////

    
	
    // 값 없을 때 팅기는 문제 때문에
    if (PreviousLocations.size() == 0) return;
	
	
    FVector previousLocation = PreviousLocations.top();
    PreviousLocations.pop();  

    if (this->GetActorLocation() != previousLocation)
    {
        this->SetActorLocation(previousLocation);
    }

    // 값 초기화는 Editor Spawn Actor에서 해주고 있음 (새로 다른 actor 클릭했을 시)
}



// Location 멀티
void AJSH_Editor_SpawnActor::Set_Location_from_Gizmo(FVector Location)
{
    Server_Set_Location_from_Gizmo(Location);
}

void AJSH_Editor_SpawnActor::Server_Set_Location_from_Gizmo_Implementation(FVector Location)
{
    NetMulti_Set_Location_from_Gizmo(Location);
}

void AJSH_Editor_SpawnActor::NetMulti_Set_Location_from_Gizmo_Implementation(FVector Location)
{
    SetActorLocation(Location);
}


// Scale 멀티
void AJSH_Editor_SpawnActor::Set_Scale_from_Gizmo(FVector Scale)
{
    Server_Set_Scale_from_Gizmo(Scale);
}

void AJSH_Editor_SpawnActor::Server_Set_Scale_from_Gizmo_Implementation(FVector Scale)
{
    NetMulti_Set_Scale_from_Gizmo(Scale);
}

void AJSH_Editor_SpawnActor::NetMulti_Set_Scale_from_Gizmo_Implementation(FVector Scale)
{
    SetActorRelativeScale3D(Scale);
}



// Rotate 멀티
void AJSH_Editor_SpawnActor::Set_Rotate_from_Gizmo(FRotator Rotate)
{
    Server_Rotate_from_Gizmo(Rotate);
}


void AJSH_Editor_SpawnActor::Server_Rotate_from_Gizmo_Implementation(FRotator Rotate)
{
    NetMulti_Rotate_from_Gizmo(Rotate);
}


void AJSH_Editor_SpawnActor::NetMulti_Rotate_from_Gizmo_Implementation(FRotator Rotate)
{
    SetActorRotation(Rotate);
}

