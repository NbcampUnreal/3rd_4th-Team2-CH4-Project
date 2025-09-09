// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;

struct FInputActionValue;

UCLASS()
class TEAM02_API ATCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	
	ATCharacter();

	virtual void Tick(float DeltaTime) override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	TObjectPtr<UCameraComponent> Camera;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	TObjectPtr<USpringArmComponent> SpringArm;

protected:

	float WalkSpeed;
	float SprintSpeed;

	// Stamina
	float MaxStamina;
	float Stamina;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float StaminaDrainRate; // 스태미너 소모 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float StaminaRecoveryRate; // 스태미너 회복 속도
	float MinSprintStamina; // 최소 스태미너
	
	bool bIsSprinting;
	bool CanSprint() const;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void SprintStart(const FInputActionValue& Value);
	void SprintStop(const FInputActionValue& Value);


};
