#include "NPC/TNpcController.h"
#include "NavigationSystem.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"

const float ATNpcController::PatrolRepeatInterval(3.f);
const float ATNpcController::PatrolRadius(500.f);

ATNpcController::ATNpcController()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ATNpcController::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		GetWorld()->GetTimerManager().SetTimer(
			PatrolTimerHandle,this,&ThisClass::OnPatrolTimerElapsed,
			PatrolRepeatInterval,true);
	}
	
}

void ATNpcController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetTimerManager().ClearTimer(PatrolTimerHandle);
	Super::EndPlay(EndPlayReason);
}

void ATNpcController::OnPatrolTimerElapsed()
{
	APawn* ControlledPawn=GetPawn();
	if (IsValid(ControlledPawn)==true)
	{
		UNavigationSystemV1* NavigationSystem=UNavigationSystemV1::GetNavigationSystem(GetWorld());
		if (IsValid(NavigationSystem)==true)
		{
			FVector ActorLocation=ControlledPawn->GetActorLocation();
			FNavLocation NextLocation;
			if (NavigationSystem->GetRandomPointInNavigableRadius(ActorLocation,PatrolRadius,NextLocation)==true)
			{
				UAIBlueprintHelperLibrary::SimpleMoveToLocation(this,NextLocation.Location);
			}
		}
	}
}

