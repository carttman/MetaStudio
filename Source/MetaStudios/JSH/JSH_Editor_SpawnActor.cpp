#include "JSH_Editor_SpawnActor.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Components/PrimitiveComponent.h"
#include "DrawDebugHelpers.h"
#include "JSH_PlayerController.h"
#include "Engine/EngineTypes.h" 
#include "GameFramework/GameModeBase.h"

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
}

// Called when the game starts
void AJSH_Editor_SpawnActor::BeginPlay()
{
    Super::BeginPlay();

    // 플레이어 컨트롤러
    JPlayerController = Cast<AJSH_PlayerController>(GetWorld()->GetFirstPlayerController());
    if (JPlayerController)
    {
        JPlayerController->bEnableTouchEvents = false;
    }

    OriginPlayer = Cast<AJSH_Player>(JPlayerController->GetPawn());
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
            AssetMesh->SetCollisionProfileName(TEXT("NoCollision"));
        }
        else
           {
            AssetMesh->SetRenderCustomDepth(false);
            AssetMesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));
            // if (GizmoActor && IsValid(GizmoActor) && GizmoActor->GetWorld())
            // {
            //     GizmoActor->Destroy();
            //     // GizmoActor->SetActorHiddenInGame(false);
            // }
        }  
    }
    
    
    // if (GizmoActor != nullptr)
    // {
    //     FVector gizmoLocation = GizmoActor->GetActorLocation();
    //     FVector currentLocation = this->GetActorLocation();
    //
    //     if (!gizmoLocation.Equals(currentLocation))
    //     {
    //         GizmoActor->SetActorTransform(this->GetActorTransform());
    //     }
    // }
}



void AJSH_Editor_SpawnActor::NotifyActorOnClicked(FKey ButtonPressed)
{
    Super::NotifyActorOnClicked(ButtonPressed);

    if (!JPlayerController) return;

    //겹쳐져 있을 때에 기즈모가 클릭되고 스폰액터는 클릭 안 되도록
    if (OriginPlayer->Gizmo_Detecting) return;

    UE_LOG(LogTemp, Error, TEXT("saved"));
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
    

    // // 이전 위치 돌아가기 위해 저장해뒀던 값을 초기화 
    // while (!OriginPlayer->PreviousLocations.empty())
    // {
    //     OriginPlayer->PreviousLocations.pop();
    // }


    GizmoSpawn();

    // else
    // {
    //     GizmoActor->SetActorHiddenInGame(false);
    // }

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
    // 1. 현재 Actor 위치 가져오기
    ThisTransform = this->GetActorTransform();

    
    // 2. 기즈모를 ThisTransform 위치에 스폰하기
    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	
    AGameModeBase* tt = Cast<AGameModeBase>(GetWorld()->GetAuthGameMode());
	
	
    // 3. 블루프린트 클래스 로드 -> 스폰
    UClass* GizmoClass = StaticLoadClass(AActor::StaticClass(), nullptr, TEXT("/Game/JSH/BP/Gizmo/BP_Gizmo.BP_Gizmo_C"));
    
    if (GizmoClass)
    {
        GizmoActor = GetWorld()->SpawnActorDeferred<AActor>(GizmoClass, ThisTransform);
        
        OriginGizmo = Cast<AJSH_Gizmo>(GizmoActor);
        
        // 원랜 켜야하는데 beginplay에서 해주고 있어서 일단 주석 처리
        //AJSH_PlayerController* PlayerController = Cast<AJSH_PlayerController>(GetWorld()->GetFirstPlayerController());

        //AActor* Gizmo = Cast<APawn>(GizmoActor);
        if (GizmoActor)
        {
            UE_LOG(LogTemp, Error, TEXT("Component 111111"));
            UGameplayStatics::FinishSpawningActor(GizmoActor, ThisTransform);

            // GizmoActor 스폰 논리
            if (GizmoActor != nullptr)
            {
                GizmoActor->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
            }
        }
    }
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
