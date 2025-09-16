#include "InGameLevel/TGameStateBase_InGame.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"

void ATGameStateBase_InGame::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ATGameStateBase_InGame, ThiefWins);
    DOREPLIFETIME(ATGameStateBase_InGame, PoliceWins);
    DOREPLIFETIME(ATGameStateBase_InGame, CurrentRound);
    DOREPLIFETIME(ATGameStateBase_InGame, RemainingSec);
}

void ATGameStateBase_InGame::StartRound(int32 RoundSeconds)
{
    RemainingSec = RoundSeconds;
    OnRep_RemainingSec(); // HUD 초기 표시

    GetWorldTimerManager().ClearTimer(RoundTimerHandle);
    GetWorldTimerManager().SetTimer(RoundTimerHandle, this, &ThisClass::TickTimer, 1.0f, true);
}

void ATGameStateBase_InGame::TickTimer()
{
    if (--RemainingSec <= 0)
    {
        GetWorldTimerManager().ClearTimer(RoundTimerHandle);
        // 승패/다음 라운드는 GameMode가 결정
    }
    OnRep_RemainingSec(); // HUD 갱신
}

void ATGameStateBase_InGame::AddWin(EInGameTeam TeamWon)
{
    if (TeamWon == EInGameTeam::Thief) ++ThiefWins; else ++PoliceWins;
    OnRep_Score(); // HUD 갱신
    ++CurrentRound;
}
