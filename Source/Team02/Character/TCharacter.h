// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class USkeletalMesh;

struct FInputActionValue;

//UENUM(BlueprintType)
//enum class ETeam : uint8
//{
//	TeamP UMETA(DisplayName = "TeamP"),
//	TeamD UMETA(DisplayName = "TeamD")
//};

UCLASS()
class TEAM02_API ATCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	ATCharacter();

	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	TObjectPtr<UCameraComponent> Camera;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	TObjectPtr<USpringArmComponent> SpringArm;

protected:

	// Stamina, Sprint
	float WalkSpeed;
	float SprintSpeed;
	float MaxStamina;
	float Stamina;
	float MinSprintStamina; // 최소 스태미너
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float StaminaDrainRate; // 스태미너 소모 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float StaminaRecoveryRate; // 스태미너 회복 속도

	bool bIsSprinting;
	bool CanSprint() const;

	// Attack
	bool bIsAttacking;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float AttackRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float AttackRadius;


	// Server

	/*UPROPERTY(ReplicatedUsing = OnRep_Team, EditAnywhere, BlueprintReadOnly, Category = "Team")
	ETeam Team = ETeam::TeamP;*/

	/*UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Team")
	TObjectPtr<USkeletalMesh> TeamPMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Team")
	TObjectPtr<USkeletalMesh> TeamDMesh;*/

	//UFUNCTION()
	//void OnRep_Team();
	//UFUNCTION(Server, Reliable)
	//void ServerSetTeam(ETeam NewTeam);
	//void ApplyTeamAppearance();
	UFUNCTION(Server, Reliable)
	void ServerAttack();
	void PerformAttack();

protected:
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void SprintStart(const FInputActionValue& Value);
	void SprintStop(const FInputActionValue& Value);
	void AttackStart(const FInputActionValue& Value);
	void AttackEnd(const FInputActionValue& Value);


};
