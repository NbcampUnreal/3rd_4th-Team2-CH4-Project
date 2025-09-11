// Fill out your copyright notice in the Description page of Project Settings.


#include "OutGameUI/TGameModeBase_Lobby.h"

#include "TUPlayerController.h"
#include "TPlayerState.h"
#include "TGameStateBase_Lobby.h"

#include "Engine/World.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"

ATGameModeBase_Lobby::ATGameModeBase_Lobby()
{
    PlayerControllerClass = ATUPlayerController::StaticClass();
    PlayerStateClass      = ATPlayerState::StaticClass();
    GameStateClass        = ATGameStateBase_Lobby::StaticClass();
    
    DefaultPawnClass      = nullptr;

    bUseSeamlessTravel    = true;
}

void ATGameModeBase_Lobby::BeginPlay()
{
    Super::BeginPlay();
    UpdateLobbyCounts();
}

void ATGameModeBase_Lobby::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);
    UpdateLobbyCounts();
    TryStartMatchIfReady();
}

void ATGameModeBase_Lobby::Logout(AController* Exiting)
{
    Super::Logout(Exiting);
    UpdateLobbyCounts();
    TryStartMatchIfReady();
}

void ATGameModeBase_Lobby::RecountLobbyAndMaybeStart()
{
    UpdateLobbyCounts();
    TryStartMatchIfReady();
}

void ATGameModeBase_Lobby::UpdateLobbyCounts()
{
    ATGameStateBase_Lobby* GS = GetGameState<ATGameStateBase_Lobby>();
    if (!GS) return;

    int32 Ready = 0;
    int32 Total = 0;

    for (APlayerState* PS : GameState->PlayerArray)
    {
        if (ATPlayerState* TPS = Cast<ATPlayerState>(PS))
        {
            ++Total;
            if (TPS->bReady)
            {
                ++Ready;
            }
        }
    }

    GS->TotalPlayers = Total;
    GS->ReadyCount   = Ready;

    UE_LOG(LogTemp, Log, TEXT("[Lobby] Ready %d / %d"), Ready, Total);
}

bool ATGameModeBase_Lobby::AreAllPlayersReady(int32& OutReady, int32& OutTotal) const
{
    OutReady = 0;
    OutTotal = 0;

    if (!GameState) return false;

    for (APlayerState* PS : GameState->PlayerArray)
    {
        if (ATPlayerState* TPS = Cast<ATPlayerState>(PS))
        {
            ++OutTotal;
            if (TPS->bReady)
            {
                ++OutReady;
            }
        }
    }

    return (OutTotal >= MinPlayersToStart) && (OutReady == OutTotal);
}

void ATGameModeBase_Lobby::TryStartMatchIfReady()
{
    int32 Ready = 0, Total = 0;
    const bool bAllReady = AreAllPlayersReady(Ready, Total);
    
    ATGameStateBase_Lobby* GS = GetGameState<ATGameStateBase_Lobby>();
    if (!GS) return;

    if (bAllReady)
    {
        if (GS->LobbyCountdown <= 0)
        {
            GS->StartCountdown(5);
        }
    }
    else
    {
        if (GS->LobbyCountdown > 0)
        {
            GS->CancelCountdown();
        }
    }
}

void ATGameModeBase_Lobby::SetPlayerReady(APlayerController* PC, bool bReady)
{
    if (!HasAuthority() || !PC) return;

    if (ATPlayerState* TPS = PC->GetPlayerState<ATPlayerState>())
    {
        if (TPS->bReady != bReady)
        {
            TPS->bReady = bReady;
            
            if (TPS->OnLobbyStatusChanged.IsBound())
            {
                TPS->OnLobbyStatusChanged.Broadcast();
            }
        }
    }
    RecountLobbyAndMaybeStart();
}

void ATGameModeBase_Lobby::StartMatchTravel()
{
    if (!HasAuthority()) return;
    
    const FString URL = NextMap + TEXT("?listen");
    GetWorld()->ServerTravel(URL, true); // Absolute=true
}