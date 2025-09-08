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
	
};
