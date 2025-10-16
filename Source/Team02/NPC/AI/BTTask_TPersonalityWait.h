#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_TPersonalityWait.generated.h"

/**
NPC의 성격 타입을 확인
성격에 맞는 대기 시간 계산
약간의 랜덤 변동 추가
Blackboard에 최종 대기 시간 저장
 */
UCLASS()
class TEAM02_API UBTTask_TPersonalityWait : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_TPersonalityWait();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp,uint8* NodeMemory) override;

private:
	UPROPERTY(EditAnywhere,Category="Personality")
	float RelaxedWaitTime=5.0f; // 여유로움 (0에 해당)

	UPROPERTY(EditAnywhere,Category="Personality")
	float NormalWaitTime=3.0f; // 보통 (1에 해당)

	UPROPERTY(EditAnywhere,Category="Personality")
	float HurryWaitTime=1.5f;  // 조급함 (2에 해당)

	UPROPERTY(EditAnywhere,Category="Personality")
	float RandomVariation=0.5f; // 개인차 변동폭
	
};
