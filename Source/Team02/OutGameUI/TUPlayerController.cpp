// TUPlayerController.cpp

#include "OutGameUI/TUPlayerController.h"
#include "OutGameUI/TGameModeBase_Lobby.h"
#include "LobbyWidget.h"
#include "Blueprint/UserWidget.h"
#include "TPlayerState.h"
#include "EngineUtils.h"
#include "Camera/CameraActor.h"
#include "TGameStateBase_Lobby.h"

ATUPlayerController::ATUPlayerController()
{
	bReplicates = true;
}

void ATUPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (!IsLocalController()) return;
	
	if (LobbyWidgetClass)
	{
		LobbyWidgetInstance = CreateWidget<ULobbyWidget>(this, LobbyWidgetClass);
		if (LobbyWidgetInstance)
		{
			LobbyWidgetInstance->AddToPlayerScreen(10000);
			LobbyWidgetInstance->SetVisibility(ESlateVisibility::Visible);
			
			RefreshLobbyFromPS();
			
			FInputModeUIOnly UIOnly;
			UIOnly.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			SetInputMode(UIOnly);
			bShowMouseCursor = true;
		}
	}
	
	for (TActorIterator<ACameraActor> It(GetWorld()); It; ++It)
	{
		SetViewTargetWithBlend(*It, 0.f);
		break;
	}

	if (ATPlayerState* TPS = GetPlayerState<ATPlayerState>())
	{
		TPS->OnLobbyStatusChanged.AddUObject(this, &ATUPlayerController::RefreshLobbyFromPS);
	}

	if (auto* GS = GetWorld()->GetGameState<ATGameStateBase_Lobby>())
	{
		GS->OnLobbyCountsChanged.AddUObject(this, &ATUPlayerController::RefreshLobbyFromPS);
	}
}

void ATUPlayerController::Server_CycleTeam_Implementation()
{
	if (ATPlayerState* TPS = GetPlayerState<ATPlayerState>())
	{
		const ETeam Next = GetNextTeam(TPS->Team);
		TPS->SetTeam(Next);          
	}
}

/*void ATUPlayerController::Server_ToggleReady_Implementation()
{
	if (ATPlayerState* TPS = GetPlayerState<ATPlayerState>())
	{
		TPS->SetReady(!TPS->bReady); 
	}
	
	if (HasAuthority())
	{
		if (ATGameModeBase_Lobby* GM = GetWorld()->GetAuthGameMode<ATGameModeBase_Lobby>())
		{
			GM->RecountLobbyAndMaybeStart();
		}
	}
}*/

void ATUPlayerController::Server_SetReady_Implementation(bool bReadyDesired)
{
	if (ATPlayerState* TPS = GetPlayerState<ATPlayerState>())
	{
		if (TPS->bReady != bReadyDesired)
		{
			TPS->SetReady(bReadyDesired);
		}
	}
	
	if (ATGameModeBase_Lobby* GM = GetWorld()->GetAuthGameMode<ATGameModeBase_Lobby>())
	{
		GM->RecountLobbyAndMaybeStart();
	}
}

void ATUPlayerController::ToggleReady()
{
	if (ATPlayerState* TPS = GetPlayerState<ATPlayerState>())
	{
		const bool NewReady = !TPS->bReady;
		Server_SetReady(NewReady);
	}
	else
	{
		Server_SetReady(true);
	}
}

void ATUPlayerController::RefreshLobbyFromPS()
{
	if (IsLocalController() && LobbyWidgetInstance)
	{
		LobbyWidgetInstance->RefreshUI();
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