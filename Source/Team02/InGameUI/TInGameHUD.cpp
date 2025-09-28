#include "InGameUI/TInGameHUD.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"   // NEW
#include "Components/VerticalBox.h"
#include "Components/Image.h"
#include "Blueprint/WidgetTree.h"
#include "GameFramework/PlayerController.h"
#include "UObject/UnrealType.h" // 리플렉션(FProperty) 접근용
#include "InGameLevel/TGameStateBase_InGame.h" // // NEW (WinsToFinish 읽기용)

void UTInGameHUD::OnTimerUpdated(int32 RemainingSec)
{
    if (!TimerText) return;
    const int32 M = RemainingSec / 60;
    const int32 S = RemainingSec % 60;
    TimerText->SetText(FText::FromString(FString::Printf(TEXT("%02d:%02d"), M, S)));
}

void UTInGameHUD::PaintWinDots(UHorizontalBox* Box, int32 Wins)
{
    if (!Box) return;
    //Box->ClearChildren();
    //for (int32 i = 0; i < 3; ++i)
    // {
    //    UImage* Dot = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
    //    실제로는 브러시(원형 텍스처) 지정. 불투명도만으로 승수 표현. 나중에 수정해야함
    //    Dot->SetOpacity(i < Wins ? 1.0f : 0.2f);
    //    Box->AddChild(Dot);
    //}

    // CHANGE: GameState의 WinsToFinish(기본 3)를 읽어서 총 개수를 결정하고,
    // i < Wins → '●', 그 외 → '○' 로 표기
    int32 WinsToFinish = 3; // 기본값
    if (const ATGameStateBase_InGame* GS = GetWorld() ? GetWorld()->GetGameState<ATGameStateBase_InGame>() : nullptr)  // NEW
    {
        WinsToFinish = GS->WinsToFinish; //  NEW
    }

    PaintTeamDots(Box, Wins, WinsToFinish); //  NEW
}

// NEW: 텍스트 도트로 그리기(필요하면 UImage로 바꿔도 됨)
void UTInGameHUD::PaintTeamDots(UHorizontalBox* Box, int32 Wins, int32 WinsToFinish)
{
    if (!Box) return;
    Box->ClearChildren(); //  NEW

    for (int32 i = 0; i < WinsToFinish; ++i) //  NEW
    {
        UTextBlock* Dot = NewObject<UTextBlock>(Box); //  NEW
        const bool bFilled = (i < Wins);              //  NEW
        Dot->SetText(FText::FromString(bFilled ? TEXT("●") : TEXT("○"))); //  NEW
        Dot->SetJustification(ETextJustify::Center);  //  NEW

        // ★ NEW: 글자 크기로 원 크기 조절
        FSlateFontInfo FontInfo = Dot->Font;
        FontInfo.Size = 64;              // 원하는 크기 (기본은 12~24 정도)
        Dot->SetFont(FontInfo);

        if (UHorizontalBoxSlot* DotSlot = Box->AddChildToHorizontalBox(Dot)) //  NEW
        {
            DotSlot->SetPadding(FMargin(2.f, 0.f)); //  NEW
        }
    }
}

void UTInGameHUD::OnScoreUpdated(int32 ThiefWins, int32 PoliceWins)
{
    PaintWinDots(ThiefWinsBox, ThiefWins);
    PaintWinDots(PoliceWinsBox, PoliceWins);

    // Round 표시는 간단히 (도둑+경찰 승수 합 + 1)로 추정
    if (RoundText)
    {
        const int32 RoundGuess = 1 + ThiefWins + PoliceWins;
        RoundText->SetText(FText::FromString(FString::Printf(TEXT("%d / 5"), FMath::Clamp(RoundGuess, 1, 5))));
    }
}

void UTInGameHUD::OnKillEvent(const FString& Killer, const FString& Victim)
{
    if (!KillLogBox) return;

    if (KillLogItemClass)
    {
        if (UUserWidget* Item = CreateWidget<UUserWidget>(GetWorld(), KillLogItemClass))
        {
            // (선택) Item BP 내부에서 Killer/Victim을 Set하는 바인딩 로직을 구현해두면 좋다.
            KillLogBox->AddChildToVerticalBox(Item);
        }
    }
    else
    {
        //  텍스트 라인만 추가 (중간 이미지 변경은 시간나면)
        UTextBlock* Line = NewObject<UTextBlock>(this);
        Line->SetText(FText::FromString(FString::Printf(TEXT("%s 🔫 %s"), *Killer, *Victim)));
        KillLogBox->AddChild(Line);
    }

    // 최근 5개 유지
    constexpr int32 MaxItems = 5;
    while (KillLogBox->GetChildrenCount() > MaxItems)
    {
        KillLogBox->RemoveChildAt(0);
    }
}

void UTInGameHUD::RefreshStaminaBar()
{
    if (!StaminaBar) return;

    // 로컬 Pawn 캐시
    if (!CachedLocalPawn.IsValid())
    {
        if (APlayerController* PC = GetOwningPlayer())
        {
            CachedLocalPawn = PC->GetPawn();
        }
    }

    float Ratio = 0.f;

    // 1) 먼저, Pawn에 "GetStaminaRatio()" UFUNCTION이 있으면 호출 (권장: 캐릭터에 BlueprintPure 게터 추가)
    if (CachedLocalPawn.IsValid())
    {
        static FName FnName(TEXT("GetStaminaRatio"));
        if (UFunction* Fn = CachedLocalPawn->FindFunction(FnName))
        {
            struct { float ReturnValue; } Params;
            CachedLocalPawn->ProcessEvent(Fn, &Params);
            Ratio = Params.ReturnValue;
        }
        else
        {
            // 2) 게터가 없다면, UPROPERTY로 노출된 "Stamina/MaxStamina"를 리플렉션으로 시도 (없으면 0으로 표시)
            float Stamina = 0.f, MaxStamina = 0.f;

            if (FProperty* StProp = CachedLocalPawn->GetClass()->FindPropertyByName(TEXT("Stamina")))
            {
                if (FFloatProperty* FP = CastField<FFloatProperty>(StProp))
                {
                    Stamina = FP->GetPropertyValue_InContainer(CachedLocalPawn.Get());
                }
            }
            if (FProperty* MaxProp = CachedLocalPawn->GetClass()->FindPropertyByName(TEXT("MaxStamina")))
            {
                if (FFloatProperty* FP = CastField<FFloatProperty>(MaxProp))
                {
                    MaxStamina = FP->GetPropertyValue_InContainer(CachedLocalPawn.Get());
                }
            }
            if (MaxStamina > 0.f) Ratio = Stamina / MaxStamina;
            // ※ 현재 프로젝트의 TCharacter에서는 스태미너 멤버가 protected로 선언되어 있어서
            //    UPROPERTY가 아니면 리플렉션으로 접근 불가할 수 있음(그 경우 0%로 보임). :contentReference[oaicite:4]{index=4}
        }
    }

    StaminaBar->SetPercent(Ratio);
}
