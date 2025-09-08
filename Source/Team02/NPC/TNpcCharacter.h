#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TNpcCharacter.generated.h"

UCLASS()
class TEAM02_API ATNpcCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ATNpcCharacter();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
