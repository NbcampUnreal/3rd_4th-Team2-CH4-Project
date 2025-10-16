#include "InGameUI/TInGamePlayerController.h"
#include "InGameUI/TInGameHUD.h"
#include "InGameLevel/TGameStateBase_InGame.h"
#include "Blueprint/UserWidget.h"
#include "TimerManager.h"

void ATInGamePlayerController::BeginPlay()
{
    Super::BeginPlay();
    if (!IsLocalController()) return;

    // 1) HUD 생성/표시
    if (InGameHUDClass)
    {
        InGameHUDInstance = CreateWidget<UTInGameHUD>(this, InGameHUDClass);
        if (InGameHUDInstance)
        {
            InGameHUDInstance->AddToViewport(1000);

            // 인게임은 보통 GameOnly 입력 모드
            FInputModeGameOnly Mode;
            SetInputMode(Mode);
            bShowMouseCursor = false;

            // 2) GameState 델리게이트 바인딩 (타이머/스코어/킬)
            if (auto GS = GetWorld()->GetGameState<ATGameStateBase_InGame>())
            {
                GS->OnTimerUpdated.AddDynamic(InGameHUDInstance, &UTInGameHUD::OnTimerUpdated);
                GS->OnScoreUpdated.AddDynamic(InGameHUDInstance, &UTInGameHUD::OnScoreUpdated);
                GS->OnKillEvent.AddDynamic(InGameHUDInstance, &UTInGameHUD::OnKillEvent);

                // 초기값으로 즉시 HUD 갱신
                InGameHUDInstance->OnTimerUpdated(GS->RemainingSec);
                InGameHUDInstance->OnScoreUpdated(GS->ThiefWins, GS->PoliceWins);
            }

            // 3) 스태미너는 캐릭터가 게터를 제공하지 않을 수 있으므로
            //    HUD에서 Pull(조회)하도록 0.1초 타이머를 돌려준다.
            FTimerHandle StaminaTick;
            GetWorld()->GetTimerManager().SetTimer(
                StaminaTick,
                FTimerDelegate::CreateWeakLambda(this, [this]()
                    {
                        if (InGameHUDInstance) InGameHUDInstance->RefreshStaminaBar();
                    }),
                0.1f, true
            );
        }
    }
}
