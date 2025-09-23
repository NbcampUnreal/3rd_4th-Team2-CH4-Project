//TGameStateBase_Lobby.cpp

#include "TGameStateBase_Lobby.h"
#include "Net/UnrealNetwork.h"
#include "TPlayerState.h"
#include "TimerManager.h"                 
#include "Engine/World.h"                 
#include "GameFramework/GameModeBase.h" 

ATGameStateBase_Lobby::ATGameStateBase_Lobby()
{
	Phase = EMatchPhase::Waiting;
	ReadyCount = 0;
	TotalPlayers = 0;
	LobbyCountdown = 0;
}

void ATGameStateBase_Lobby::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATGameStateBase_Lobby, Phase);
	DOREPLIFETIME(ATGameStateBase_Lobby, ReadyCount);
	DOREPLIFETIME(ATGameStateBase_Lobby, TotalPlayers);
	DOREPLIFETIME(ATGameStateBase_Lobby, LobbyCountdown);
}

void ATGameStateBase_Lobby::OnRep_Phase()
{
}

void ATGameStateBase_Lobby::OnRep_Counts()
{
	OnLobbyCountsChanged.Broadcast();
}

void ATGameStateBase_Lobby::AddPlayerState(APlayerState* PS)
{
	Super::AddPlayerState(PS);

	if (HasAuthority())
	{
		if (ATPlayerState* TPS = Cast<ATPlayerState>(PS))
		{
			TPS->OnLobbyStatusChanged.AddLambda([this]()
			{
				if (HasAuthority()) RecalcCounts();
			});
		}
		RecalcCounts();
	}
}
void ATGameStateBase_Lobby::RemovePlayerState(APlayerState* PS)
{
	Super::RemovePlayerState(PS);

	if (HasAuthority())
	{
		RecalcCounts();
	}
}

void ATGameStateBase_Lobby::RecalcCounts()
{
	int32 Ready = 0;
	int32 Total = PlayerArray.Num();

	for (APlayerState* PS : PlayerArray)
	{
		if (const ATPlayerState* TPS = Cast<ATPlayerState>(PS))
		{
			if (TPS->bReady) Ready++;
		}
	}

	ReadyCount  = Ready;
	TotalPlayers = Total;
	
	OnRep_Counts();
	
	ForceNetUpdate();
}

void ATGameStateBase_Lobby::OnRep_LobbyCountdown()
{
	OnLobbyCountsChanged.Broadcast();
}


void ATGameStateBase_Lobby::StartCountdown(int32 Seconds)
{
	if (!HasAuthority()) return;
	UE_LOG(LogTemp, Warning, TEXT("[Lobby] Countdown start: %d"), Seconds);
	
	GetWorld()->GetTimerManager().ClearTimer(CountdownHandle);
	
	Phase = EMatchPhase::Countdown;
	
	LobbyCountdown = FMath::Max(Seconds, 0);
	
	GetWorld()->GetTimerManager().SetTimer(
		CountdownHandle, this, &ATGameStateBase_Lobby::TickCountdown, 1.0f, true
	);
	
	ForceNetUpdate();
}


void ATGameStateBase_Lobby::CancelCountdown()
{
	if (!HasAuthority()) return;

	GetWorld()->GetTimerManager().ClearTimer(CountdownHandle);
	LobbyCountdown = 0;
	Phase = EMatchPhase::Waiting;

	ForceNetUpdate();
}


void ATGameStateBase_Lobby::TickCountdown()
{
	if (!HasAuthority()) return;
	UE_LOG(LogTemp, Warning, TEXT("[Lobby] Countdown tick: %d"), LobbyCountdown);
	
	LobbyCountdown = FMath::Max(LobbyCountdown - 1, 0);
	
	if (LobbyCountdown <= 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(CountdownHandle);
		Phase = EMatchPhase::Traveling;

		NotifyCountdownFinished();
	}
	
	ForceNetUpdate();
}


void ATGameStateBase_Lobby::NotifyCountdownFinished()
{
	UE_LOG(LogTemp, Warning, TEXT("[Lobby] Countdown finished → StartMatchTravel()"));
	if (!HasAuthority()) return;

	if (AGameModeBase* GM = GetWorld()->GetAuthGameMode())
	{
	
		if (UFunction* Fn = GM->FindFunction(TEXT("StartMatchTravel")))
		{
			GM->ProcessEvent(Fn, nullptr);
		}
	}
}