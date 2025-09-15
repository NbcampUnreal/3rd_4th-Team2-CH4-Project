// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/TCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Character/TPlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine/SkeletalMesh.h"
#include "OutGameUI/TPlayerState.h"
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
	DOREPLIFETIME(ATCharacter, SpeedBuffMultiplier);
}


// Called to bind functionality to input
void ATCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up action bindings

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (ATPlayerController* TPC = Cast<ATPlayerController>(GetController()))
		{
			if (TPC->MoveAction)
			{
				EnhancedInputComponent->BindAction(TPC->MoveAction, ETriggerEvent::Triggered, this, &ATCharacter::Move);
			}
			if (TPC->LookAction)
			{
				EnhancedInputComponent->BindAction(TPC->LookAction, ETriggerEvent::Triggered, this, &ATCharacter::Look);
			}
			if (TPC->SprintAction)
			{
				EnhancedInputComponent->BindAction(TPC->SprintAction, ETriggerEvent::Started, this, &ATCharacter::SprintStart);
				EnhancedInputComponent->BindAction(TPC->SprintAction, ETriggerEvent::Completed, this, &ATCharacter::SprintStop);
			}
			if (TPC->AttackAction)
			{
				EnhancedInputComponent->BindAction(TPC->AttackAction, ETriggerEvent::Started, this, &ATCharacter::AttackStart);
				EnhancedInputComponent->BindAction(TPC->AttackAction, ETriggerEvent::Completed, this, &ATCharacter::AttackEnd);
			}
			if (TPC->Skill1Action)
			{
				EnhancedInputComponent->BindAction(TPC->Skill1Action, ETriggerEvent::Started, this, &ATCharacter::UseSkill);
			}
			if (TPC->Skill2Action)
			{
				EnhancedInputComponent->BindAction(TPC->Skill2Action, ETriggerEvent::Started, this, &ATCharacter::UseSkill2);
			}
		}

	}
}

void ATCharacter::BeginPlay()
{
	Super::BeginPlay();
	bCanUseBindSkill = true;
	bCanUseSpeedupSkill = true;
	UpdateTeamTags();
}

void ATCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
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


	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;

	if (!bIsSprinting && CanSprint())
	{
		bIsSprinting = true;
		if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
		{
			MoveComp->MaxWalkSpeed = SprintSpeed;
		}
	}


}

void ATCharacter::SprintStop(const FInputActionValue& Value)
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	if (bIsSprinting)
	{
		bIsSprinting = false;
	}
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->MaxWalkSpeed = WalkSpeed;
	}
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

	ServerAttack();

}

void ATCharacter::ServerAttack_Implementation()
{


	PerformAttack();

}

void ATCharacter::PerformAttack()
{
	//if (Team != ETeam::TeamP)
	//{
	//	UE_LOG(LogTemp, Verbose, TEXT("Attack denied (server): Not TeamP"));

	//	return; // 공격은 TeamP만
	//}


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

void ATCharacter::Server_UseSkill_Implementation()
{
	if (!bCanUseBindSkill) return;

	ATPlayerState* PS = GetPlayerState<ATPlayerState>();
	if (PS && PS->Team == ETeam::Thief)
	{
		if (BindSkillActorClass)
		{
			UWorld* World = GetWorld();
			if (World)
			{
				bCanUseBindSkill = false;
				const FVector SpawnLocation = GetActorLocation();
				const FRotator SpawnRotation = GetActorRotation();
				FActorSpawnParameters SpawnParams;
				SpawnParams.Owner = this;
				SpawnParams.Instigator = GetInstigator();

				World->SpawnActor<ATBind>(BindSkillActorClass, SpawnLocation, SpawnRotation, SpawnParams);
			}
		}
	}
}

void ATCharacter::Server_UseSkill2_Implementation()
{
	if (!bCanUseSpeedupSkill) return;

	ATPlayerState* PS = GetPlayerState<ATPlayerState>();
	if (PS && PS->Team == ETeam::Thief)
	{
		if (SpeedupSkillActorClass)
		{
			UWorld* World = GetWorld();
			if (World)
			{
				bCanUseSpeedupSkill = false;
				const FVector SpawnLocation = GetActorLocation();
				const FRotator SpawnRotation = GetActorRotation();
				FActorSpawnParameters SpawnParams;
				SpawnParams.Owner = this;
				SpawnParams.Instigator = GetInstigator();

				World->SpawnActor<ATSpeedup>(SpeedupSkillActorClass, SpawnLocation, SpawnRotation, SpawnParams);
			}
		}
	}
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
		MoveComp->MaxWalkSpeed = WalkSpeed * SpeedBuffMultiplier;
	}
}

void ATCharacter::UpdateTeamTags()
{
	ATPlayerState* PS = GetPlayerState<ATPlayerState>();
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

