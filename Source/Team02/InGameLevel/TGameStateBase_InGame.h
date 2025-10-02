#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "OutGameUI/TTeamTypes.h" 
#include "TGameStateBase_InGame.generated.h"

// - RemainingSec: 라운드 남은 시간(초)
// - ThiefWins / PoliceWins: 3선승 스코어
// - 델리게이트: HUD로 신호 전송 (타이머/스코어/킬)
 

UENUM(BlueprintType)
enum class EInGameTeam : uint8 { Thief, Police };

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnScoreUpdated, int32, ThiefWins, int32, PoliceWins);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTimerUpdated, int32, RemainingSec);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnKillEvent, const FString&, Killer, const FString&, Victim);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMatchFinished, EInGameTeam, WinnerTeam);   

// ---- 킬로그 데이터 ----
USTRUCT(BlueprintType)                                            // NEW
struct FKillLogEntry
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly) ETeam KillerTeam = ETeam::None;  // NEW
    UPROPERTY(BlueprintReadOnly) ETeam VictimTeam = ETeam::None;  // NEW
    UPROPERTY(BlueprintReadOnly) bool bVictimIsAI = false;                     // NEW
    UPROPERTY(BlueprintReadOnly) float ServerTime = 0.f;                       // NEW
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnKillLogEvent, const FKillLogEntry&, Entry); // NEW


UCLASS()
class TEAM02_API ATGameStateBase_InGame : public AGameStateBase
{
    GENERATED_BODY()
public:
    // 스코어/라운드
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

    //타이머
    UPROPERTY(ReplicatedUsing=OnRep_RemainingSec, BlueprintReadOnly, Category="Timer")
    int32 RemainingSec = 180;

    // 킬로그
    UPROPERTY(BlueprintAssignable) FOnKillLogEvent OnKillLog;      // NEW

    UFUNCTION(NetMulticast, Reliable)                               // NEW
        void MulticastKillLog(const FKillLogEntry& Entry);              // NEW

    //  HUD 델리게이트
    UPROPERTY(BlueprintAssignable) FOnScoreUpdated OnScoreUpdated;
    UPROPERTY(BlueprintAssignable) FOnTimerUpdated OnTimerUpdated;
    UPROPERTY(BlueprintAssignable) FOnKillEvent    OnKillEvent;

    UPROPERTY(BlueprintAssignable) FOnMatchFinished  OnMatchFinished;                        

    // RepNotify → HUD 즉시 반영
    UFUNCTION() void OnRep_Score()        { OnScoreUpdated.Broadcast(ThiefWins, PoliceWins); }
    UFUNCTION() void OnRep_RemainingSec() { OnTimerUpdated.Broadcast(RemainingSec); }

    //  서버 제어
    void StartRound(int32 RoundSeconds);
    void TickTimer();
    void AddWin(EInGameTeam TeamWon);
    void BroadcastKill(const FString& Killer, const FString& Victim) { OnKillEvent.Broadcast(Killer, Victim); }

    UFUNCTION(BlueprintCallable)
    void ResetMatchState(int32 InWinsToFinish = 3, int32 InMaxRounds = 5, int32 InRoundSeconds = 180);

    // 매치 종료 제어(서버에서 호출)                                             
    void FinishMatch(EInGameTeam Winner);      



protected:
    UFUNCTION() void OnRep_MatchFinished();                                                

private:
    FTimerHandle RoundTimerHandle;

    // 매치 종료 복제 상태                                                        
    UPROPERTY(ReplicatedUsing = OnRep_MatchFinished) bool bMatchFinished = false;           
    UPROPERTY(Replicated) EInGameTeam MatchWinner = EInGameTeam::Police;                  

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
