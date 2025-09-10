//TGameStateBase_Lobby.cpp

#include "TGameStateBase_Lobby.h"
#include "Net/UnrealNetwork.h"

ATGameStateBase_Lobby::ATGameStateBase_Lobby()
{
	Phase = EMatchPhase::Waiting;
	ReadyCount = 0;
	TotalPlayers = 0;
	LobbyCountdown = 0;
}

void ATGameStateBase_Lobby::OnRep_Phase()
{
	// Phase 변경 시 처리 로직 (UI 업데이트 등)
}

void ATGameStateBase_Lobby::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATGameStateBase_Lobby, Phase);
	DOREPLIFETIME(ATGameStateBase_Lobby, ReadyCount);
	DOREPLIFETIME(ATGameStateBase_Lobby, TotalPlayers);
	DOREPLIFETIME(ATGameStateBase_Lobby, LobbyCountdown);
}