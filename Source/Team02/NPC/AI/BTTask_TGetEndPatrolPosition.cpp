#include "NPC/AI/BTTask_TGetEndPatrolPosition.h"
#include "NPC/TNpcController.h"
#include "NPC/TNpcCharacter.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"


UBTTask_TGetEndPatrolPosition::UBTTask_TGetEndPatrolPosition()
{
	NodeName=TEXT("GetEndPatrolPosition");
}

EBTNodeResult::Type UBTTask_TGetEndPatrolPosition::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp,NodeMemory);

	if (EBTNodeResult::Failed==Result)
	{
		return Result;
	}

	ATNpcController* NpcController=Cast<ATNpcController>(OwnerComp.GetAIOwner());
	checkf(IsValid(NpcController)==true,TEXT("Invalid NpcController"));

	ATNpcCharacter* NPC=Cast<ATNpcCharacter>(NpcController->GetPawn());
	checkf(IsValid(NPC)==true,TEXT("Invalid NPC"));

	UNavigationSystemV1* NavigationSystem=UNavigationSystemV1::GetNavigationSystem(NPC->GetWorld());
	checkf(IsValid(NavigationSystem)==true,TEXT("Invalid NavigationSystem"));

	FVector StartPatrolPosition=OwnerComp.GetBlackboardComponent()->GetValueAsVector(ATNpcController::StartPatrolPositionKey);
	FNavLocation EndPatrolLocation;
	if (true==NavigationSystem->GetRandomPointInNavigableRadius(StartPatrolPosition,NpcController->PatrolRadius,EndPatrolLocation))
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(ATNpcController::EndPatrolPositionKey,EndPatrolLocation.Location);
		return Result=EBTNodeResult::Succeeded;
	}
	return Result;
}
