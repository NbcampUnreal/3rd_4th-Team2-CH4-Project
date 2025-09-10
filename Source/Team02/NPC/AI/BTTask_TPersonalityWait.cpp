#include "NPC/AI/BTTask_TPersonalityWait.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NPC/TNpcController.h"


UBTTask_TPersonalityWait::UBTTask_TPersonalityWait()
{
	NodeName=TEXT("Personality Wait");
}

EBTNodeResult::Type UBTTask_TPersonalityWait::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackboardComp=OwnerComp.GetBlackboardComponent();
	if (BlackboardComp==nullptr)
		return EBTNodeResult::Failed;
	
	int32 PersonalityType=BlackboardComp->GetValueAsInt(ATNpcController::PersonalityTypeKey); //성격 타입 가져오기
	float BaseWaitTime=3.0f; //기본값

	switch (PersonalityType) //성격별 대기 시간 설정
	{
	case 0:
		BaseWaitTime=RelaxedWaitTime;
		break;
	case 1:
		BaseWaitTime=NormalWaitTime;
		break;
	case 2:
		BaseWaitTime=HurryWaitTime;
		break;
	default:
		BaseWaitTime=NormalWaitTime;
		break;
	}

	//개인차 변동 적용
	float FinalWaitTime=BaseWaitTime + FMath::RandRange(-RandomVariation,RandomVariation);
	FinalWaitTime=FMath::Max(0.5f,FinalWaitTime);

	BlackboardComp->SetValueAsFloat(ATNpcController::WaitTimeKey,FinalWaitTime); // wait노드에서 사용할수 있게 블랙보드에 저장

	return EBTNodeResult::Succeeded;
	
}
