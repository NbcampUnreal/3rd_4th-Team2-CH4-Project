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
	bCountdownActive = false;
}

void ATGameStateBase_Lobby::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATGameStateBase_Lobby, Phase);
	DOREPLIFETIME(ATGameStateBase_Lobby, ReadyCount);
	DOREPLIFETIME(ATGameStateBase_Lobby, TotalPlayers);
	DOREPLIFETIME(ATGameStateBase_Lobby, LobbyCountdown);
	DOREPLIFETIME(ATGameStateBase_Lobby, bCountdownActive);
}

void ATGameStateBase_Lobby::OnRep_Phase()
{
	bCountdownActive = (Phase == EMatchPhase::Countdown) && (LobbyCountdown > 0);
	OnLobbyCountsChanged.Broadcast();
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

bool ATGameStateBase_Lobby::AreAllPlayersReady() const
{
	return (TotalPlayers >= 2 && ReadyCount == TotalPlayers);
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

	if (HasAuthority())
	{
		const bool bAllReady = AreAllPlayersReady();
		if (bAllReady)
		{
			if (!bCountdownActive)
			{
				StartCountdown(5);
			}
		}
		else
		{
			if (bCountdownActive || LobbyCountdown > 0)
			{
				CancelCountdown();
			}
		}
	}
	
	OnRep_Counts();
	
	ForceNetUpdate();
}

void ATGameStateBase_Lobby::OnRep_LobbyCountdown()
{
	bCountdownActive = (Phase == EMatchPhase::Countdown) && (LobbyCountdown > 0);
	OnLobbyCountsChanged.Broadcast();
}


void ATGameStateBase_Lobby::StartCountdown(int32 Seconds)
{
	if (!HasAuthority()) return;

	if (bCountdownActive && LobbyCountdown > 0) return;
	UE_LOG(LogTemp, Warning, TEXT("[Lobby] Countdown start: %d"), Seconds);
	
	GetWorld()->GetTimerManager().ClearTimer(CountdownHandle);
	
	Phase = EMatchPhase::Countdown;
	
	LobbyCountdown = FMath::Max(Seconds, 0);
	bCountdownActive = (LobbyCountdown > 0);
	
	GetWorld()->GetTimerManager().SetTimer(
		CountdownHandle, this, &ATGameStateBase_Lobby::TickCountdown, 1.0f, true
	);
	
	ForceNetUpdate();
	OnLobbyCountsChanged.Broadcast();
}


void ATGameStateBase_Lobby::CancelCountdown()
{
	if (!HasAuthority()) return;

	GetWorld()->GetTimerManager().ClearTimer(CountdownHandle);
	
	LobbyCountdown = 0;
	bCountdownActive = false;  
	Phase = EMatchPhase::Waiting;

	ForceNetUpdate();
	OnLobbyCountsChanged.Broadcast();
}


void ATGameStateBase_Lobby::TickCountdown()
{
	if (!HasAuthority()) return;

	if (!AreAllPlayersReady())
	{
		CancelCountdown();
		return;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("[Lobby] Countdown tick: %d"), LobbyCountdown);
	
	LobbyCountdown = FMath::Max(LobbyCountdown - 1, 0);
	bCountdownActive = (LobbyCountdown > 0);
	
	if (LobbyCountdown <= 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(CountdownHandle);

		bCountdownActive = false;
		Phase = EMatchPhase::Traveling;

		NotifyCountdownFinished();
	}
	
	ForceNetUpdate();
	OnLobbyCountsChanged.Broadcast();
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