#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/HorizontalBox.h"
#include "Components/TextBlock.h"
#include "InGameLevel/TGameStateBase_InGame.h"  
#include "TInGameHUD.generated.h"

class UProgressBar; class UTextBlock; class UHorizontalBox; class UVerticalBox;

/**
 * 인게임 HUD: "표시"만 담당.
 * - 스태미너 ProgressBar (캐릭터 값 Pull)
 * - 라운드 타이머 MM:SS
 * - 5판 3선승 스코어 ●●●
 * - 킬 로그 리스트
 *
 * 실제 규칙(시간 흐름/점수/킬 판정)은 GameMode/GameState/Character에서 수행.
 */
UCLASS()
class TEAM02_API UTInGameHUD : public UUserWidget
{
    GENERATED_BODY()
public:
    // GameState에서 브로드캐스트되는 이벤트 수신
    UFUNCTION() void OnTimerUpdated(int32 RemainingSec);
    UFUNCTION() void OnScoreUpdated(int32 ThiefWins, int32 PoliceWins);
    UFUNCTION() void OnKillEvent(const FString& Killer, const FString& Victim);

    // 캐릭터의 스태미너 비율을 Pull하여 ProgressBar 갱신 
    UFUNCTION(BlueprintCallable, Category = "UI") void RefreshStaminaBar();

    virtual void NativeConstruct() override;
    // --- KillLog UI ---
    UPROPERTY(meta = (BindWidget)) UVerticalBox* KillLogBox = nullptr; //  (WBP_InGameHUD 안의 KillLogBox와 이름 일치)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "KillLog")
    int32 MaxKillLines = 6;                                        // ★ NEW
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "KillLog")
    float LineLifeSeconds = 4.0f;                                  // ★ NEW

    UFUNCTION() void HandleKillLogToHUD(const FKillLogEntry& Entry);   // ★ NEW

protected:
    // BindWidget: BP_InGameHUD에서 동일한 이름으로 위젯을 배치해야 자동 연결됨
    UPROPERTY(meta = (BindWidget)) UProgressBar* StaminaBar = nullptr;
    UPROPERTY(meta = (BindWidget)) UTextBlock* TimerText = nullptr;
    UPROPERTY(meta = (BindWidget)) UTextBlock* RoundText = nullptr;
    UPROPERTY(meta = (BindWidget)) UHorizontalBox* ThiefWinsBox = nullptr;
    UPROPERTY(meta = (BindWidget)) UHorizontalBox* PoliceWinsBox = nullptr;
    

    //(옵션) 킬로그 한 줄짜리 BP 위젯 클래스 
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UUserWidget> KillLogItemClass;

private:
    // 로컬 캐릭터 캐싱 (스태미너 Pull 용)
    TWeakObjectPtr<AActor> CachedLocalPawn;

    // 내부: ●●● 점으로 3선승 표시
    
    // 기존 함수 시그니처 유지하되 동작이 바뀜(총 개수=WinsToFinish, ○/●로 그림)
    void PaintWinDots(UHorizontalBox* Box, int32 Wins); // // CHANGE: 내부 로직 변경(고정 3개 → WinsToFinish개, ○/●)

    // 새 헬퍼: 팀 한 줄을 WinsToFinish개 도트로 그리기(○/●)
    void PaintTeamDots(UHorizontalBox* Box, int32 Wins, int32 WinsToFinish); 

    // --- KillLog UI ---
    void AddKillLogTextLine(const FKillLogEntry& Entry);           // ★ NEW
    static FString TeamToLabel(ETeam Team);                        // ★ NEW
};
