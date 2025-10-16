// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Gimmick/TSpeedup.h"
#include "OutGameUI/TTeamTypes.h"
#include "InGameLevel/TPlayerState_InGame.h"
#include "Gimmick/TBell.h" // Added for Police Bell Gimmick
#include "TCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class USkeletalMesh;
class ATBind;
// class APlayerState;

struct FInputActionValue;

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
	virtual void OnRep_PlayerState() override;
	virtual void PossessedBy(AController* NewController) override;

	void ApplySpeedBuff(float Multiplier, float Duration);

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	TObjectPtr<UCameraComponent> Camera;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	TSubclassOf<ATBind> BindSkillActorClass;

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	TSubclassOf<ATSpeedup> SpeedupSkillActorClass;

	UPROPERTY(Replicated)
	bool bCanUseBindSkill;

	UPROPERTY(Replicated)
	bool bCanUseSpeedupSkill;

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	TSubclassOf<class ATBell> BellSkillActorClass;

	UPROPERTY(Replicated)
	bool bCanUseBellSkill;

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
	UFUNCTION(BlueprintPure, Category = "Stamina")
	float GetStaminaRatio() const { return (MaxStamina > 0.f) ? (Stamina / MaxStamina) : 0.f; }  //  NEW // 스태미너 게터 

	bool bIsSprinting;
	bool CanSprint() const;
	bool bSprintLocked = false; //  회복 중 재스프린트 금지

	// Attack
	bool bIsAttacking;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float AttackRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float AttackRadius;

	// Speed Buff
	UPROPERTY(ReplicatedUsing = OnRep_SpeedBuffMultiplier)
	float SpeedBuffMultiplier;
	FTimerHandle SpeedBuffTimerHandle;

	UFUNCTION()
	void OnRep_SpeedBuffMultiplier();

	// Server
	UFUNCTION(Server, Reliable)
	void ServerAttack();
	void PerformAttack();

	UFUNCTION(Server, Reliable)
	void Server_UseSkill();

	UFUNCTION(Server, Reliable)
	void Server_UseSkill2();

	UFUNCTION(Server, Reliable)
	void Server_UseSkill3();

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void SprintStart(const FInputActionValue& Value);
	void SprintStop(const FInputActionValue& Value);
	void AttackStart(const FInputActionValue& Value);
	void AttackEnd(const FInputActionValue& Value);
	void UseSkill(const FInputActionValue& Value);
	void UseSkill2(const FInputActionValue& Value);

	void UseSkill3(const FInputActionValue& Value);

	void UpdateTeamTags();
	void EndSpeedBuff();
	void UpdateMovementSpeed();

	UPROPERTY(EditDefaultsOnly, Category = "Team|Mesh")
	TObjectPtr<USkeletalMesh> Mesh_None;
	UPROPERTY(EditDefaultsOnly, Category = "Team|Mesh")
	TObjectPtr<USkeletalMesh> Mesh_Police;
	UPROPERTY(EditDefaultsOnly, Category = "Team|Mesh")
	TObjectPtr<USkeletalMesh> Mesh_Thief;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	TObjectPtr <class UAnimMontage> MeleeAttackMontage;
	void AttackMontagePlay();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	TObjectPtr <class UAnimMontage> RunningMontage;
	void RunningMontagePlay();

	bool bHasAppliedTeam = false;
	ETeam LastAppliedTeam = ETeam::None;
	TWeakObjectPtr<ATPlayerState_InGame> BoundPlayerState;

};