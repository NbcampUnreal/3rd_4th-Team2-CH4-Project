// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/TCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "OutGameUI/TUPlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine/SkeletalMesh.h"
#include "InGameLevel/TPlayerState_InGame.h"
#include "OutGameUI/TTeamTypes.h"
#include "Gimmick/TBind.h"
#include "Gimmick/TSpeedup.h"

// Sets default values
ATCharacter::ATCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	SetReplicateMovement(true);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 400.f;
	SpringArm->bUsePawnControlRotation = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
	Camera->bUsePawnControlRotation = false;

	WalkSpeed = 600.f;
	SprintSpeed = 1200.f;

	MaxStamina = 100.f;
	Stamina = MaxStamina;
	StaminaDrainRate = 100.f;
	StaminaRecoveryRate = 5.f;
	MinSprintStamina = 5.f;
	bIsSprinting = false;

	AttackRange = 200.f;
	AttackRadius = 50.f;
	bIsAttacking = false;

	SpeedBuffMultiplier = 1.0f;
	bCanUseBindSkill = true;
	bCanUseSpeedupSkill = true;
}

void ATCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATCharacter, bCanUseBindSkill);
	DOREPLIFETIME(ATCharacter, bCanUseSpeedupSkill);
	DOREPLIFETIME(ATCharacter, bCanUseBellSkill);
	DOREPLIFETIME(ATCharacter, SpeedBuffMultiplier);
}


// Called to bind functionality to input
void ATCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		UE_LOG(LogTemp, Warning, TEXT("[Char] SetupPlayerInputComponent"));

		// (당신 프로젝트 구조에 맞게) 컨트롤러에서 액션 가져오는 경우:
		if (ATUPlayerController* PC = Cast<ATUPlayerController>(GetController()))
		{
			UE_LOG(LogTemp, Warning, TEXT("[Char] Actions  Move:%s  Look:%s  Sprint:%s  Attack:%s"),
				PC->MoveAction ? *PC->MoveAction->GetName() : TEXT("NULL"),
				PC->LookAction ? *PC->LookAction->GetName() : TEXT("NULL"),
				PC->SprintAction ? *PC->SprintAction->GetName() : TEXT("NULL"),
				PC->AttackAction ? *PC->AttackAction->GetName() : TEXT("NULL"));

			// 여기서 실제 바인딩 (이미 있다면 로그만 유지)
			if (PC->MoveAction)   EIC->BindAction(PC->MoveAction, ETriggerEvent::Triggered, this, &ATCharacter::Move);
			if (PC->LookAction)   EIC->BindAction(PC->LookAction, ETriggerEvent::Triggered, this, &ATCharacter::Look);
			EIC->BindAction(PC->SprintAction, ETriggerEvent::Started, this, &ATCharacter::SprintStart);
			EIC->BindAction(PC->SprintAction, ETriggerEvent::Completed, this, &ATCharacter::SprintStop);
			if (PC->AttackAction)
			{
				EIC->BindAction(PC->AttackAction, ETriggerEvent::Started, this, &ATCharacter::AttackStart);
				EIC->BindAction(PC->AttackAction, ETriggerEvent::Completed, this, &ATCharacter::AttackEnd);
			}
			if (PC->Skill1Action)
			{
				EIC->BindAction(PC->Skill1Action, ETriggerEvent::Started, this, &ATCharacter::UseSkill);
			}
			if (PC->Skill2Action)
			{
				EIC->BindAction(PC->Skill2Action, ETriggerEvent::Started, this, &ATCharacter::UseSkill2);
			}
			if (PC->Skill3Action)
			{
				EIC->BindAction(PC->Skill3Action, ETriggerEvent::Started, this, &ATCharacter::UseSkill3);
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[Char] Controller is not TUPlayerController"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[Char] EnhancedInputComponent not found"));
	}
}

void ATCharacter::BeginPlay()
{
	Super::BeginPlay();
	bCanUseBindSkill = true;
	bCanUseSpeedupSkill = true;
	bCanUseBellSkill = true;
}

void ATCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	UpdateTeamTags();
}

void ATCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// Server-side: PlayerState is guaranteed to be valid here
	UpdateTeamTags();
}

void ATCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsSprinting)
	{
		Stamina -= StaminaDrainRate * DeltaTime;
		UE_LOG(LogTemp, Warning, TEXT("Stamina: %f"), Stamina);
		if (Stamina <= 0.f)
		{
			Stamina = 0.f;
			SprintStop(FInputActionValue(false));
		}
	}
	else
	{
		if (Stamina < MaxStamina)
		{
			Stamina += StaminaRecoveryRate * DeltaTime;
			if (Stamina > MaxStamina)
			{
				Stamina = MaxStamina;
			}
		}
	}
}

void ATCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();
	if (Controller != nullptr)
	{

		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		// get forward vector
		const FVector ForwardDirection = GetActorForwardVector(); //FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(ForwardDirection, MovementVector.Y);
		// get right vector 
		const FVector RightDirection = GetActorRightVector(); //FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ATCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);
}

bool ATCharacter::CanSprint() const
{
	return Stamina >= MinSprintStamina;
}

void ATCharacter::SprintStart(const FInputActionValue& Value)
{
	if (!bIsSprinting && CanSprint())
	{
		RunningMontagePlay();
		bIsSprinting = true;
		UCharacterMovementComponent* MoveComp = GetCharacterMovement();
		if (MoveComp)
		{
			MoveComp->MaxWalkSpeed = SprintSpeed;
		}
		//UpdateMovementSpeed();스피드업이 적용이 안되서 일단 주석처리함
	}
}

void ATCharacter::SprintStop(const FInputActionValue& Value)
{
	if (bIsSprinting)
	{
		bIsSprinting = false;
	}
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->MaxWalkSpeed = WalkSpeed;
	}
	//UpdateMovementSpeed();스피드업이 적용이 안되서 일단 주석처리함
}

void ATCharacter::AttackStart(const FInputActionValue& Value)
{

	if (bIsAttacking)
	{
		return;
	}
	if (bIsSprinting)
	{
		SprintStop(FInputActionValue(false));
	}
	bIsAttacking = true;
	AttackMontagePlay();

	ServerAttack();

}

void ATCharacter::ServerAttack_Implementation()
{
	// 팀 Police만 공격 가능
	// 테스트용으로 잠시 막음
	/*ATPlayerState_InGame* PS = GetPlayerState<ATPlayerState_InGame>();
	if (!PS || PS->Team != ETeam::Police)
	{
		UE_LOG(LogTemp, Verbose, TEXT("[Char] Attack denied (server): Only Police can attack"));
		return;
	}*/
	PerformAttack();

}

void ATCharacter::PerformAttack()
{
	const FVector Forward = GetActorForwardVector();
	const FVector Start = GetActorLocation() + Forward * 100.f;
	const FVector End = Start + Forward * AttackRange;

	FCollisionShape Shape = FCollisionShape::MakeSphere(AttackRadius);

	FCollisionQueryParams Params(SCENE_QUERY_STAT(MeleeAttack), /*bTraceComplex*/ false, this);
	Params.AddIgnoredActor(this);

	FCollisionObjectQueryParams ObjParams;
	ObjParams.AddObjectTypesToQuery(ECC_Pawn); // Pawn 대상만

	TArray<FHitResult> Hits;
	const bool bHit = GetWorld()->SweepMultiByObjectType(Hits, Start, End, FQuat::Identity, ObjParams, Shape, Params);

	if (bHit)
	{
		for (const FHitResult& HR : Hits)
		{
			AActor* Other = HR.GetActor();
			if (Other && Other != this)
			{
				UE_LOG(LogTemp, Log, TEXT("Melee hit: %s"), *Other->GetName());

				// 한방에 제거(서버 권한)
				Other->Destroy();

				break; // 첫 타겟만
			}
		}
	}

}

void ATCharacter::AttackEnd(const FInputActionValue& Value)
{
	bIsAttacking = false;
}

void ATCharacter::UseSkill(const FInputActionValue& Value)
{
	Server_UseSkill();
}

void ATCharacter::UseSkill2(const FInputActionValue& Value)
{
	Server_UseSkill2();
}

void ATCharacter::UseSkill3(const FInputActionValue& Value)
{
	Server_UseSkill3();
}

void ATCharacter::Server_UseSkill_Implementation()
{
	if (!bCanUseBindSkill)
	{
		return;
	}

	ATPlayerState_InGame* PS = GetPlayerState<ATPlayerState_InGame>();
	if (!PS)
	{
		return;
	}

	if (!BindSkillActorClass)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	bCanUseBindSkill = false;	const FVector SpawnLocation = GetActorLocation();
	const FRotator SpawnRotation = GetActorRotation();
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();

	World->SpawnActor<ATBind>(BindSkillActorClass, SpawnLocation, SpawnRotation, SpawnParams);
}

void ATCharacter::Server_UseSkill2_Implementation()
{
	if (!bCanUseSpeedupSkill)
	{
		return;
	}

	ATPlayerState_InGame* PS = GetPlayerState<ATPlayerState_InGame>();
	if (!PS)
	{
		return;
	}

	if (!SpeedupSkillActorClass)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	bCanUseSpeedupSkill = false;
	const FVector SpawnLocation = GetActorLocation();
	const FRotator SpawnRotation = GetActorRotation();
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();

	World->SpawnActor<ATSpeedup>(SpeedupSkillActorClass, SpawnLocation, SpawnRotation, SpawnParams);
}

void ATCharacter::Server_UseSkill3_Implementation()
{
	if (!bCanUseBellSkill)
	{
		return;
	}

	ATPlayerState_InGame* PS = GetPlayerState<ATPlayerState_InGame>(); // IMPORTANT: This still needs to be ATPlayerState_InGame
	if (!PS)
	{
		return;
	}


	if (!BellSkillActorClass)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	bCanUseBellSkill = false;
	const FVector SpawnLocation = GetActorLocation();
	const FRotator SpawnRotation = GetActorRotation();
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();

	World->SpawnActor<ATBell>(BellSkillActorClass, SpawnLocation, SpawnRotation, SpawnParams);
}

void ATCharacter::ApplySpeedBuff(float Multiplier, float Duration)
{
	if (HasAuthority())
	{
		SpeedBuffMultiplier = Multiplier;
		OnRep_SpeedBuffMultiplier(); // Server needs to call this manually
		GetWorld()->GetTimerManager().SetTimer(SpeedBuffTimerHandle, this, &ATCharacter::EndSpeedBuff, Duration, false);
	}
}

void ATCharacter::EndSpeedBuff()
{
	if (HasAuthority())
	{
		SpeedBuffMultiplier = 1.0f;
		OnRep_SpeedBuffMultiplier(); // Server needs to call this manually
	}
}

void ATCharacter::OnRep_SpeedBuffMultiplier()
{
	UpdateMovementSpeed();
}

void ATCharacter::UpdateMovementSpeed()
{
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		float CurrentBaseSpeed = bIsSprinting ? SprintSpeed : WalkSpeed;
		MoveComp->MaxWalkSpeed = CurrentBaseSpeed * SpeedBuffMultiplier;
	}
}

void ATCharacter::UpdateTeamTags()
{
	ATPlayerState_InGame* PS = GetPlayerState<ATPlayerState_InGame>();
	if (!PS) return;

	// Remove existing tags first
	Tags.Remove(FName("Tagger"));
	Tags.Remove(FName("Hider"));

	if (PS->Team == ETeam::Police)
	{
		Tags.Add(FName("Tagger"));
	}
	else if (PS->Team == ETeam::Thief)
	{
		Tags.Add(FName("Hider"));
	}
}

void ATCharacter::AttackMontagePlay()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->Montage_Play(MeleeAttackMontage);
}

void ATCharacter::RunningMontagePlay()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->Montage_Play(RunningMontage);
}