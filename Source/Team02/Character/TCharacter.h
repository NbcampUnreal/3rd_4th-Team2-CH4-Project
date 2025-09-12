// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "OutGameUI/TTeamTypes.h"
#include "TCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class USkeletalMesh;
class ATPlayerState;
struct FInputActionValue;

UCLASS()
class TEAM02_API ATCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	ATCharacter();

	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void OnRep_PlayerState() override;
	virtual void PossessedBy(AController* NewController) override;

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

	UFUNCTION(Server, Reliable)
	void ServerAttack();
	void PerformAttack();

	UPROPERTY(EditDefaultsOnly, Category = "Team|Mesh")
	TObjectPtr<USkeletalMesh> Mesh_None;
	UPROPERTY(EditDefaultsOnly, Category = "Team|Mesh")
	TObjectPtr<USkeletalMesh> Mesh_Police;
	UPROPERTY(EditDefaultsOnly, Category = "Team|Mesh")
	TObjectPtr<USkeletalMesh> Mesh_Thief;

	bool bHasAppliedTeam = false;
	ETeam LastAppliedTeam = ETeam::None;
	TWeakObjectPtr<ATPlayerState> BoundPlayerState;

	void BindTeamDelegate(); // 팀 변경 델리게이트 바인딩
	void SyncTeamAppearance(); // 플레이어 상태와 외형 동기화
	void ApplyTeamAppearance(ETeam NewTeam); // 팀 외형 적용

protected:
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void SprintStart(const FInputActionValue& Value);
	void SprintStop(const FInputActionValue& Value);
	void AttackStart(const FInputActionValue& Value);
	void AttackEnd(const FInputActionValue& Value);


};

