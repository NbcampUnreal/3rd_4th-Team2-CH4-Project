#include "NPC/TNpcController.h"
#include "NavigationSystem.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetSystemLibrary.h"

const float ATNpcController::PatrolRadius(500.f);
int32 ATNpcController::ShowAIDebug(0);
const FName ATNpcController::StartPatrolPositionKey(TEXT("StartPatrolPosition"));
const FName ATNpcController::EndPatrolPositionKey(TEXT("EndPatrolPosition"));

FAutoConsoleVariableRef CVarShowAIDebug(
	TEXT("NXProject.ShowAIDebug"),
	ATNpcController::ShowAIDebug,
	TEXT(""),
	ECVF_Cheat);

ATNpcController::ATNpcController()
{
	UE_LOG(LogTemp,Warning,TEXT("TNpcController Constructor Called"));
	
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
		}
		if (ShowAIDebug==1)
		{
			UKismetSystemLibrary::PrintString(this,FString::Printf(TEXT("BeginAI")));
		}
	}
}

void ATNpcController::EndAI()
{
	UBehaviorTreeComponent* BehaviorTreeComponent=Cast<UBehaviorTreeComponent>(BrainComponent);
	if (IsValid(BehaviorTreeComponent)==true)
	{
		BehaviorTreeComponent->StopTree();

		if (ShowAIDebug==1)
		{
			UKismetSystemLibrary::PrintString(this,FString::Printf(TEXT("EndAI")));
		}
	}
}

