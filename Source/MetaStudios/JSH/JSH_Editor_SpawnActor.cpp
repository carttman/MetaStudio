#include "JSH_Editor_SpawnActor.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Components/PrimitiveComponent.h"
#include "DrawDebugHelpers.h"
#include "JSH_PlayerController.h"
#include "Engine/EngineTypes.h" 
#include "GameFramework/GameModeBase.h"
#include "Kismet/KismetSystemLibrary.h"  

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


    AssetMesh->OnClicked.AddDynamic(this, &AJSH_Editor_SpawnActor::OnMeshClicked);
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
            if (GizmoActor != nullptr)
            {
                GizmoActor->Destroy();
            }
        }  
    }
}


void AJSH_Editor_SpawnActor::OnMeshClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed)
{
    // APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    // if (!PlayerController) return;
    //
    //
    // FHitResult HitResult;
    // bool bHit = PlayerController->GetHitResultUnderCursorByChannel(static_cast<ETraceTypeQuery>(ECC_Visibility), true, HitResult);

    if (!JPlayerController) return;
    
    FHitResult HitResult;
    bool bHit = JPlayerController->GetHitResultUnderCursorByChannel(static_cast<ETraceTypeQuery>(ECC_Visibility), true, HitResult);

    
    // 클릭 했을때 자신의 정보를 PlayerController에 저장
    //JPlayerController->SaveEditorActor(this);
    
    UE_LOG(LogTemp, Error, TEXT("Click"));
    // 클릭 했을때 자신의 정보를 Player에 저장
    OriginPlayer->SaveEditorActor(this);
    GizmoSpawn();

    UE_LOG(LogTemp, Error, TEXT("Click"));
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
    UClass* GizmoClass = StaticLoadClass(AActor::StaticClass(), nullptr, TEXT("/Game/JSH/BP/Gizmo/BP_Gizmo_Scale.BP_Gizmo_Scale_C"));
    
    if (GizmoClass)
    {
        GizmoActor = GetWorld()->SpawnActorDeferred<AActor>(GizmoClass, ThisTransform);

        // 원랜 켜야하는데 beginplay에서 해주고 있어서 일단 주석 처리
        //AJSH_PlayerController* PlayerController = Cast<AJSH_PlayerController>(GetWorld()->GetFirstPlayerController());

        //AActor* Gizmo = Cast<APawn>(GizmoActor);
        if (GizmoActor)
        {
            UGameplayStatics::FinishSpawningActor(GizmoActor, ThisTransform);
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
