#include "NPC/TNpcBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"

ATNpcBase::ATNpcBase()
{
	PrimaryActorTick.bCanEverTick = false;

	//멀티 플레이어 설정
	bReplicates=true;
	AutoPossessAI=EAutoPossessAI::PlacedInWorldOrSpawned; // AI컨르롤러 할당
	AutoPossessPlayer=EAutoReceiveInput::Disabled; // 플레이어 입력방지
	GetCharacterMovement()->SetIsReplicated(true); //네트워크 동기화
	SetReplicateMovement(true);
	//NetCullDistanceSquared=150000.f;
	
	
	float CharacterHalfHeight=90.f;
	float CharacterRadius=40.f;

	GetCapsuleComponent()->InitCapsuleSize(CharacterRadius,CharacterHalfHeight);

	FVector PivotPosition(0.f,0.f,-CharacterHalfHeight);
	FRotator PivotRotation(0.f,-90.f,0.f);
	GetMesh()->SetRelativeLocationAndRotation(PivotPosition,PivotRotation);

	GetCharacterMovement()->MaxWalkSpeed=350.f;
	GetCharacterMovement()->MinAnalogWalkSpeed=20.f;
	GetCharacterMovement()->JumpZVelocity=700.f;
	GetCharacterMovement()->AirControl=0.35f;
	GetCharacterMovement()->BrakingDecelerationWalking=2000.f;
	
}

void ATNpcBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

