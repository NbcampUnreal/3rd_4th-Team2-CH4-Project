#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "TGameStateBase_InGame.generated.h"

/**
 * 인게임의 "진실 소스":
 * - RemainingSec: 라운드 남은 시간(초)
 * - ThiefWins / PoliceWins: 3선승 스코어
 * - 델리게이트: HUD로 신호 전송 (타이머/스코어/킬)
 */

UENUM(BlueprintType)
enum class EInGameTeam : uint8 { Thief, Police };

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnScoreUpdated, int32, ThiefWins, int32, PoliceWins);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTimerUpdated, int32, RemainingSec);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnKillEvent, const FString&, Killer, const FString&, Victim);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMatchFinished, EInGameTeam, WinnerTeam);      // NEW

UCLASS()
class TEAM02_API ATGameStateBase_InGame : public AGameStateBase
{
    GENERATED_BODY()
public:
    // ── 스코어/라운드
    UPROPERTY(ReplicatedUsing=OnRep_Score, BlueprintReadOnly, Category="Score")
    int32 ThiefWins = 0;

    UPROPERTY(ReplicatedUsing=OnRep_Score, BlueprintReadOnly, Category="Score")
    int32 PoliceWins = 0;

    UPROPERTY(Replicated, BlueprintReadOnly, Category="Score")
    int32 CurrentRound = 1;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Score")
    int32 MaxRounds = 5;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Score")
    int32 WinsToFinish = 3;

    // ── 타이머
    UPROPERTY(ReplicatedUsing=OnRep_RemainingSec, BlueprintReadOnly, Category="Timer")
    int32 RemainingSec = 180;

    // ── HUD 델리게이트
    UPROPERTY(BlueprintAssignable) FOnScoreUpdated OnScoreUpdated;
    UPROPERTY(BlueprintAssignable) FOnTimerUpdated OnTimerUpdated;
    UPROPERTY(BlueprintAssignable) FOnKillEvent    OnKillEvent;

    UPROPERTY(BlueprintAssignable) FOnMatchFinished  OnMatchFinished;                         // NEW

    // RepNotify → HUD 즉시 반영
    UFUNCTION() void OnRep_Score()        { OnScoreUpdated.Broadcast(ThiefWins, PoliceWins); }
    UFUNCTION() void OnRep_RemainingSec() { OnTimerUpdated.Broadcast(RemainingSec); }

    // ── 서버 제어
    void StartRound(int32 RoundSeconds);
    void TickTimer();
    void AddWin(EInGameTeam TeamWon);
    void BroadcastKill(const FString& Killer, const FString& Victim) { OnKillEvent.Broadcast(Killer, Victim); }

    // 매치 종료 제어(서버에서 호출)                                                //  NEW
    void FinishMatch(EInGameTeam Winner);                                                   //  NEW

protected:
    UFUNCTION() void OnRep_MatchFinished();                                                 //  NEW

private:
    FTimerHandle RoundTimerHandle;

    // 매치 종료 복제 상태                                                         //  NEW
    UPROPERTY(ReplicatedUsing = OnRep_MatchFinished) bool bMatchFinished = false;            //  NEW
    UPROPERTY(Replicated) EInGameTeam MatchWinner = EInGameTeam::Police;                   //  NEW

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
