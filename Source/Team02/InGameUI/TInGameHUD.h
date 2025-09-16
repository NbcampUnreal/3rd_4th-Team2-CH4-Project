#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
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

protected:
    // BindWidget: BP_InGameHUD에서 동일한 이름으로 위젯을 배치해야 자동 연결됨
    UPROPERTY(meta = (BindWidget)) UProgressBar* StaminaBar = nullptr;
    UPROPERTY(meta = (BindWidget)) UTextBlock* TimerText = nullptr;
    UPROPERTY(meta = (BindWidget)) UTextBlock* RoundText = nullptr;
    UPROPERTY(meta = (BindWidget)) UHorizontalBox* ThiefWinsBox = nullptr;
    UPROPERTY(meta = (BindWidget)) UHorizontalBox* PoliceWinsBox = nullptr;
    UPROPERTY(meta = (BindWidget)) UVerticalBox* KillLogBox = nullptr;

    //(옵션) 킬로그 한 줄짜리 BP 위젯 클래스 
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UUserWidget> KillLogItemClass;

private:
    // 로컬 캐릭터 캐싱 (스태미너 Pull 용)
    TWeakObjectPtr<AActor> CachedLocalPawn;

    // 내부: ●●● 점으로 3선승 표시
    void PaintWinDots(UHorizontalBox* Box, int32 Wins);
};
