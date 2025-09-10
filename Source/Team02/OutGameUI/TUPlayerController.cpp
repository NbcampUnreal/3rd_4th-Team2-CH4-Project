// TUPlayerController.cpp


#include "OutGameUI/TUPlayerController.h"

#include "TRootHUDWidget.h"
#include "Blueprint/UserWidget.h"
#include "TPlayerState.h"

ATUPlayerController::ATUPlayerController()
{
	bReplicates = true;
}

void ATUPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController() && RootHUDClass)
	{
		RootHUD = CreateWidget<UTRootHudWidget>(this, RootHUDClass);
		if (RootHUD)
		{
			RootHUD->AddToViewport();
			RefreshLobbyFromPS();

			// 로비에서 마우스 사용(원하면)
			FInputModeUIOnly UIOnly;
			UIOnly.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			SetInputMode(UIOnly);
			bShowMouseCursor = true;
		}
	}
}

void ATUPlayerController::Server_CycleTeam_Implementation()
{
	if (ATPlayerState* TPS = GetPlayerState<ATPlayerState>())
	{
		TPS->SetTeam(GetNextTeam(TPS->Team));
	}
}

void ATUPlayerController::Server_ToggleReady_Implementation()
{
	if (ATPlayerState* TPS = GetPlayerState<ATPlayerState>())
	{
		TPS->SetReady(!TPS->bReady);
	}
}

void ATUPlayerController::RefreshLobbyFromPS()
{
	if (IsLocalController() && RootHUD)
	{
		RootHUD->RefreshLobby();
	}
}

ETeam ATUPlayerController::GetNextTeam(ETeam Current) const
{
	switch (Current)
	{
	case ETeam::None:   return ETeam::Police;
	case ETeam::Police: return ETeam::Thief;
	case ETeam::Thief:  return ETeam::None;
	default:            return ETeam::None;
	}
}