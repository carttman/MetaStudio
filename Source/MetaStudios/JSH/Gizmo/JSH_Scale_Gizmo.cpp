// CustomActor.cpp
#include "../../JSH/Gizmo/JSH_Scale_Gizmo.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

AJSH_Scale_Gizmo::AJSH_Scale_Gizmo()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AJSH_Scale_Gizmo::BeginPlay()
{
	Super::BeginPlay();

}

void AJSH_Scale_Gizmo::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}
