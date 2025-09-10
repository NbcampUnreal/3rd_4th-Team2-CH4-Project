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
    // 로비에서 사용할 클래스 매핑
    PlayerControllerClass = ATUPlayerController::StaticClass();
    PlayerStateClass      = ATPlayerState::StaticClass();
    GameStateClass        = ATGameStateBase_Lobby::StaticClass();

    // 로비에서는 파운 스폰이 필요 없을 수 있음(대기실만)
    DefaultPawnClass      = nullptr;

    // 선택: 로비에서는 관전자 모드로 시작하고 싶다면 주석 해제
    // bStartPlayersAsSpectators = true;
}

void ATGameModeBase_Lobby::BeginPlay()
{
    Super::BeginPlay();

    // 맵이 로비로 제대로 열렸는지, 클래스 매핑이 적용됐는지 점검할 때 유용
    // UE_LOG(LogTemp, Log, TEXT("Lobby GM BeginPlay. MinPlayers=%d Auto=%s"),
    //     MinPlayersToStart, bAutoStartWhenAllReady ? TEXT("true") : TEXT("false"));

    UpdateLobbyCounts();
}

void ATGameModeBase_Lobby::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    // 새 플레이어 접속 → 인원/레디 수 갱신
    UpdateLobbyCounts();

    if (bAutoStartWhenAllReady)
    {
        TryStartMatchIfReady();
    }
}

void ATGameModeBase_Lobby::Logout(AController* Exiting)
{
    Super::Logout(Exiting);

    // 플레이어 퇴장 → 인원/레디 수 갱신
    UpdateLobbyCounts();

    // 누가 빠져나가면 자동 시작 조건이 깨질 수 있으므로 별도 처리 필요 없음
}

void ATGameModeBase_Lobby::RecountLobbyAndMaybeStart()
{
    UpdateLobbyCounts();

    if (bAutoStartWhenAllReady)
    {
        TryStartMatchIfReady();
    }
}

void ATGameModeBase_Lobby::UpdateLobbyCounts()
{
    ATGameStateBase_Lobby* GS = GetGameState<ATGameStateBase_Lobby>();
    if (!GS) return;

    int32 Ready = 0;
    int32 Total = 0;

    // GameState의 PlayerArray에는 현재 접속 중인 모든 PlayerState가 들어 있음
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

    // 서버에서 집계한 값을 로비 GameState에 반영(자동 복제됨)
    GS->TotalPlayers = Total;
    GS->ReadyCount   = Ready;

    // 로비 단계 보장
    GS->Phase = EMatchPhase::Waiting; 

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

    // 최소 인원 충족 + 전원 Ready
    return (OutTotal >= MinPlayersToStart) && (OutReady == OutTotal);
}

void ATGameModeBase_Lobby::TryStartMatchIfReady()
{
    int32 Ready = 0, Total = 0;
    if (!AreAllPlayersReady(Ready, Total))
    {
        return;
    }

    // "매치 시작" 처리
    if (ATGameStateBase_Lobby* GS = GetGameState<ATGameStateBase_Lobby>())
    {
        GS->Phase = EMatchPhase::Loading; 
    }
    
}