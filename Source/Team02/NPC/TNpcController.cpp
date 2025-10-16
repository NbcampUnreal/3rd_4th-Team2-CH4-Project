#include "NPC/TNpcController.h"
#include "NavigationSystem.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"

const float ATNpcController::PatrolRadius(500.f);
const FName ATNpcController::StartPatrolPositionKey(TEXT("StartPatrolPosition"));
const FName ATNpcController::EndPatrolPositionKey(TEXT("EndPatrolPosition"));
const FName ATNpcController::PersonalityTypeKey(TEXT("PersonalityType"));
const FName ATNpcController::WaitTimeKey(TEXT("WaitTime"));

ATNpcController::ATNpcController()
{
	Blackboard=CreateDefaultSubobject<UBlackboardComponent>(TEXT("Blackboard"));
	BrainComponent=CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BrainComponent"));
}

void ATNpcController::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority()==true)
	{
		APawn* ControlledPawn=GetPawn();
		if (IsValid(ControlledPawn)==true)
		{
			BeginAI(ControlledPawn);
		}
	}
}

void ATNpcController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	EndAI();
	Super::EndPlay(EndPlayReason);
}

void ATNpcController::BeginAI(APawn* InPawn)
{
	UBlackboardComponent* BlackboardComponent = Cast<UBlackboardComponent>(Blackboard);
	if (IsValid(BlackboardComponent)==true)
	{
		if (UseBlackboard(BlackboardDataAsset,BlackboardComponent)==true)
		{
			bool bRunSucceeded=RunBehaviorTree(BehaviorTree);
			checkf(bRunSucceeded==true,TEXT("Fail to run behavior tree"));
			BlackboardComponent->SetValueAsVector(StartPatrolPositionKey,InPawn->GetActorLocation());

			//성격 유형 추가 (0=여유로움,1=보통,2=조급함)
			int32 RandomPersonalityType=FMath::RandRange(0,2);
			BlackboardComponent->SetValueAsInt(PersonalityTypeKey,RandomPersonalityType);
		}
	}
}

void ATNpcController::EndAI()
{
	UBehaviorTreeComponent* BehaviorTreeComponent=Cast<UBehaviorTreeComponent>(BrainComponent);
	if (IsValid(BehaviorTreeComponent)==true)
	{
		BehaviorTreeComponent->StopTree();
	}
}

