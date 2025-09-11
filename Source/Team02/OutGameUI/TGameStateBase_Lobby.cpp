//TGameStateBase_Lobby.cpp

#include "TGameStateBase_Lobby.h"
#include "Net/UnrealNetwork.h"
#include "TPlayerState.h"

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
	// Phase 변경 시 처리 로직 (UI 업데이트 등)
}

void ATGameStateBase_Lobby::OnRep_Counts()
{
	// 클라에서 합계 텍스트 갱신 트리거
	OnLobbyCountsChanged.Broadcast();
}

void ATGameStateBase_Lobby::AddPlayerState(APlayerState* PS)
{
	Super::AddPlayerState(PS);

	if (HasAuthority())
	{
		// 플레이어 상태가 변할 때마다 합계 재계산
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
	// 서버에서 PlayerArray 돌며 합계 산출
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

	// 서버 로컬에서도 즉시 UI가 필요하면 Rep 핸들러 직접 호출
	OnRep_Counts();

	// 복제 빠르게
	ForceNetUpdate();
}