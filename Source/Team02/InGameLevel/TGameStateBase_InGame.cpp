#include "InGameLevel/TGameStateBase_InGame.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"
#include "InGameLevel/TGameModeBase_InGame.h"   //  NEW (시간 만료 통지용)

void ATGameStateBase_InGame::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ATGameStateBase_InGame, ThiefWins);
    DOREPLIFETIME(ATGameStateBase_InGame, PoliceWins);
    DOREPLIFETIME(ATGameStateBase_InGame, CurrentRound);
    DOREPLIFETIME(ATGameStateBase_InGame, RemainingSec);
    DOREPLIFETIME(ATGameStateBase_InGame, bMatchFinished);                //  NEW
    DOREPLIFETIME(ATGameStateBase_InGame, MatchWinner);                   //  NEW
}

void ATGameStateBase_InGame::StartRound(int32 RoundSeconds)
{
    // 매치가 끝난 뒤에는 더 이상 시작하지 않음                               // ★ NEW
    if (bMatchFinished) return;

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

         // ★ NEW: 시간 만료 → GameMode에 통지(Thief 승리 규칙)
        if (HasAuthority()) // 서버에서만
        {
            if (ATGameModeBase_InGame* GM = GetWorld()->GetAuthGameMode<ATGameModeBase_InGame>()) //  NEW
            {
                GM->HandleRoundTimeOver(); //  NEW  ( GameMode에 구현)
            }
        }
        return;
    }
    OnRep_RemainingSec(); // HUD 갱신
}

void ATGameStateBase_InGame::AddWin(EInGameTeam TeamWon)
{
    if (TeamWon == EInGameTeam::Thief) ++ThiefWins; else ++PoliceWins;
    OnRep_Score(); // HUD 갱신
    ++CurrentRound;
}

//  매치 종료(서버에서 호출)
void ATGameStateBase_InGame::FinishMatch(EInGameTeam Winner)                                //  NEW
{
    if (!HasAuthority()) return;                                                             //  NEW
    bMatchFinished = true;                                                                   //  NEW
    MatchWinner = Winner;                                                                    //  NEW
    GetWorldTimerManager().ClearTimer(RoundTimerHandle);                                     //  NEW
    OnRep_MatchFinished();                                                                   //  NEW
    ForceNetUpdate();                                                                        //  NEW
}

//  매치 종료 복제 통지 → 클라 HUD/PC가 결과 화면으로 전환
void ATGameStateBase_InGame::OnRep_MatchFinished()                                           //  NEW
{
    if (bMatchFinished)
    {
        OnMatchFinished.Broadcast(MatchWinner);                                              //  NEW
    }
}
