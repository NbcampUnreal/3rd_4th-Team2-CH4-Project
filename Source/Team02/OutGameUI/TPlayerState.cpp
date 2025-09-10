// TPlayerState.cpp


#include "TPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/PlayerController.h"
#include "TUPlayerController.h"

ATPlayerState::ATPlayerState()
{
}

void ATPlayerState::SetTeam(ETeam NewTeam)
{
	if (HasAuthority())
	{
		Team = NewTeam;
		OnRep_Team(); // 서버 로컬에서도 즉시 UI 반영
	}
}

void ATPlayerState::SetReady(bool bInReady)
{
	if (HasAuthority())
	{
		bReady = bInReady;
		OnRep_Ready();
	}
}

void ATPlayerState::OnRep_Team()
{
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