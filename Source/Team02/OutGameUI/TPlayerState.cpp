// TPlayerState.cpp

#include "TPlayerState.h"
#include "InGameLevel/TPlayerState_InGame.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/PlayerController.h"
#include "TUPlayerController.h"

ATPlayerState::ATPlayerState() {}

void ATPlayerState::SetTeam(ETeam NewTeam)
{
	if (HasAuthority())
	{
		Team = NewTeam;
		OnRep_Team();
		ForceNetUpdate();
	}
	else
	{
		ServerSetTeam(NewTeam);
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
		ServerSetReady(bInReady);
	}
}

void ATPlayerState::ServerSetTeam_Implementation(ETeam NewTeam)
{
	if (Team != NewTeam)
	{
		Team = NewTeam;
		OnRep_Team();
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

void ATPlayerState::OnRep_Team()
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

void ATPlayerState::CopyProperties(APlayerState* NewPlayerState)
{
	Super::CopyProperties(NewPlayerState);

	if (ATPlayerState_InGame* InGamePS = Cast<ATPlayerState_InGame>(NewPlayerState))
	{
		InGamePS->Team = this->Team;
	}
}