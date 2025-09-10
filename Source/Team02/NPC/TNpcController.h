#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "TNpcController.generated.h"

class UBlackboardData;
class UBehaviorTree;

/**
 * 
 */
UCLASS()
class TEAM02_API ATNpcController : public AAIController
{
	GENERATED_BODY()

public:
	ATNpcController();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	void BeginAI(APawn* InPawn);
	void EndAI();

public:
	static const float PatrolRadius;
	static int32 ShowAIDebug;

	static const FName StartPatrolPositionKey;
	static const FName EndPatrolPositionKey;
	static const FName PersonalityTypeKey; // 성격유형키 (조급함,보통,여유로움)
	static const FName WaitTimeKey;

private:
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Meta=(AllowPrivateAccess))
	TObjectPtr<UBlackboardData> BlackboardDataAsset;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Meta=(AllowPrivateAccess))
	TObjectPtr<UBehaviorTree> BehaviorTree;
	
};
