#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"
#include "TNpcBase.generated.h"

UCLASS()
class TEAM02_API ATNpcBase : public ACharacter
{
	GENERATED_BODY()

public:
	ATNpcBase();

protected:
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
};
