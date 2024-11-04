#include "JSH_Editor_SpawnActor.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Components/PrimitiveComponent.h"
#include "DrawDebugHelpers.h"
#include "JSH_PlayerController.h"
#include "Engine/EngineTypes.h" 
#include "Kismet/KismetSystemLibrary.h"  

// Constructor: Set default values
AJSH_Editor_SpawnActor::AJSH_Editor_SpawnActor()
{
    PrimaryActorTick.bCanEverTick = true; 

    RootScence = CreateDefaultSubobject<USceneComponent>(TEXT("RootScence"));
    RootComponent = RootScence;
    
    AssetMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body"));
    AssetMesh->SetupAttachment(RootComponent);
   


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

    UE_LOG(LogTemp, Error, TEXT("Click"));
    // 한번 클릭시 자기 자신 삭제
    // if (bHit && HitResult.GetActor() == this)
    // {
    //     DrawDebugSphere(GetWorld(), HitResult.Location, 10.0f, 12, FColor::Green, false, 2.0f);
    //
    //     Destroy();
    // }
}

void AJSH_Editor_SpawnActor::Destroy_This()
{
    // if (JPlayerController->GetName() == this->GetName())
    // {
    //     Destroy();    
    // }
}
