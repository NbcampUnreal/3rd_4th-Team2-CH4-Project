// TUPlayerController.cpp

#include "OutGameUI/TUPlayerController.h"
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

	// 로비 위젯 바로 생성/표시
	if (LobbyWidgetClass)
	{
		LobbyWidgetInstance = CreateWidget<ULobbyWidget>(this, LobbyWidgetClass);
		if (LobbyWidgetInstance)
		{
			LobbyWidgetInstance->AddToPlayerScreen(10000);
			LobbyWidgetInstance->SetVisibility(ESlateVisibility::Visible);

			// 첫 화면 값 갱신
			RefreshLobbyFromPS();

			// 입력을 UI로
			FInputModeUIOnly UIOnly;
			UIOnly.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			SetInputMode(UIOnly);
			bShowMouseCursor = true;
		}
	}

	// (선택) 로비에서 Pawn이 없으면 화면이 검정 → 임시 카메라 잡기
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

void ATUPlayerController::Server_ToggleReady_Implementation()
{
	if (ATPlayerState* TPS = GetPlayerState<ATPlayerState>())
	{
		TPS->SetReady(!TPS->bReady); 
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