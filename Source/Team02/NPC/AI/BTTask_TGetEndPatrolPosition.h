#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_TGetEndPatrolPosition.generated.h"

/**
 * 
 */
UCLASS()
class TEAM02_API UBTTask_TGetEndPatrolPosition : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_TGetEndPatrolPosition();

private:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
