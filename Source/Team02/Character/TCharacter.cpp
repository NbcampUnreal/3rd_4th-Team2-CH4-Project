// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/TCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "TPlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine/SkeletalMesh.h"
#include "OutGameUI/TPlayerState.h"
#include "OutGameUI/TTeamTypes.h"

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
		}

	}
}

void ATCharacter::BeginPlay()
{
	Super::BeginPlay();
	BindTeamDelegate();
	SyncTeamAppearance();
}

void ATCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	BindTeamDelegate();
	SyncTeamAppearance();
}
void ATCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	BindTeamDelegate();
	SyncTeamAppearance();
}

void ATCharacter::BindTeamDelegate()
{
	ATPlayerState* PS = GetPlayerState<ATPlayerState>();
	if (!PS) return;

	// PlayerState가 바뀌었으면 새로 바인딩
	if (BoundPlayerState.Get() != PS)
	{
		// 기존 것은 TWeakObjectPtr 로 자연소멸 (Unbind 불필요 - AddUObject 는 객체 파괴 시 자동 해제)
		PS->OnLobbyStatusChanged.AddUObject(this, &ATCharacter::SyncTeamAppearance);
		BoundPlayerState = PS;
	}
}

void ATCharacter::SyncTeamAppearance()
{
	ETeam Current = ETeam::None;
	if (ATPlayerState* PS = GetPlayerState<ATPlayerState>())
	{
		Current = PS->Team;
	}
	ApplyTeamAppearance(Current);
}

void ATCharacter::ApplyTeamAppearance(ETeam NewTeam)
{
	// 1) 변경 없음이면 빠르게 종료
	if (bHasAppliedTeam && LastAppliedTeam == NewTeam)
	{
		return;
	}

	USkeletalMeshComponent* MeshComp = GetMesh();
	if (!MeshComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("ApplyTeamAppearance: MeshComp is null."));
		return;
	}

	// 2) 적용할 메쉬 선택
	USkeletalMesh* Desired = nullptr;
	switch (NewTeam)
	{
	case ETeam::Police: Desired = Mesh_Police; break;
	case ETeam::Thief:  Desired = Mesh_Thief;  break;
	case ETeam::None:
	default:            Desired = Mesh_None;   break;
	}

	// 3) None 상태이거나 지정 메쉬가 비어있으면 기존 메쉬 유지 (로그는 1회만)
	if (!Desired)
	{
		if (!bHasAppliedTeam) // 최초 적용 시에만 경고 (반복 경고 방지)
		{
			UE_LOG(LogTemp, Warning, TEXT("ApplyTeamAppearance: Desired mesh is null for team %d. Keeping current mesh."), static_cast<int32>(NewTeam));
		}
		Desired = MeshComp->GetSkeletalMeshAsset();
	}

	// 4) 실제 변경 필요 여부 확인
	if (Desired && MeshComp->GetSkeletalMeshAsset() != Desired)
	{
		MeshComp->SetSkeletalMesh(Desired, /*bReinitPose*/ true);
		// 필요 시: 팀별 머티리얼/애님BP/속도 조정 등을 여기서 확장
		// 예) if (UCharacterMovementComponent* Move = GetCharacterMovement()) { ... }
		UE_LOG(LogTemp, Log, TEXT("ApplyTeamAppearance: Mesh changed to %s (Team %d)"),
			*Desired->GetName(), static_cast<int32>(NewTeam));
	}

	// 5) 상태 갱신
	LastAppliedTeam = NewTeam;
	bHasAppliedTeam = true;
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