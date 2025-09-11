// TPlayerState.cpp

#include "TPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/PlayerController.h"
#include "TUPlayerController.h"

ATPlayerState::ATPlayerState() {}

void ATPlayerState::SetTeam(ETeam NewTeam)
{
	if (HasAuthority())
	{
		Team = NewTeam;
		OnRep_Team();       // 서버 로컬 즉시 UI 반영
		ForceNetUpdate();   // 복제 빠르게
	}
	else
	{
		ServerSetTeam(NewTeam); // ★ 클라 → 서버
	}
}

void ATPlayerState::SetReady(bool bInReady)
{
	if (HasAuthority())
	{
		bReady = bInReady;
		OnRep_Ready();
		ForceNetUpdate();
	}
	else
	{
		ServerSetReady(bInReady); // ★ 클라 → 서버
	}
}

/** ===== 서버 RPC 구현 필수 (_Implementation) ===== */
void ATPlayerState::ServerSetTeam_Implementation(ETeam NewTeam)
{
	if (Team != NewTeam)
	{
		Team = NewTeam;
		OnRep_Team();      // 서버에서도 즉시 반영
		ForceNetUpdate();
	}
}
void ATPlayerState::ServerSetReady_Implementation(bool bInReady)
{
	if (bReady != bInReady)
	{
		bReady = bInReady;
		OnRep_Ready();
		ForceNetUpdate();
	}
}

/** ===== RepNotify: 클라에서 UI 갱신 트리거 ===== */
void ATPlayerState::OnRep_Team()
{
	// (선호) 델리게이트로도 알림 주기
	OnLobbyStatusChanged.Broadcast();

	if (APlayerController* PC = Cast<APlayerController>(GetOwner()))
	{
		if (ATUPlayerController* TPC = Cast<ATUPlayerController>(PC))
		{
			TPC->RefreshLobbyFromPS();
		}
	}
}

void ATPlayerState::OnRep_Ready()
{
	OnLobbyStatusChanged.Broadcast();

	if (APlayerController* PC = Cast<APlayerController>(GetOwner()))
	{
		if (ATUPlayerController* TPC = Cast<ATUPlayerController>(PC))
		{
			TPC->RefreshLobbyFromPS();
		}
	}
}

void ATPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATPlayerState, Team);
	DOREPLIFETIME(ATPlayerState, bReady);
}