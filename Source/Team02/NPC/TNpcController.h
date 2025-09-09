#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "TNpcController.generated.h"

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

private:
	void OnPatrolTimerElapsed();

public:
	FTimerHandle PatrolTimerHandle;

	static const float PatrolRepeatInterval;

	static const float PatrolRadius;
	
};
