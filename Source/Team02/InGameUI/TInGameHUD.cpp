#include "InGameUI/TInGameHUD.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"   
#include "Components/VerticalBox.h"
#include "Components/Image.h"
#include "Blueprint/WidgetTree.h"
#include "GameFramework/PlayerController.h"
#include "UObject/UnrealType.h" // 리플렉션(FProperty) 접근용
#include "InGameLevel/TGameStateBase_InGame.h" // (WinsToFinish 읽기용)

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
    if (const ATGameStateBase_InGame* GS = GetWorld() ? GetWorld()->GetGameState<ATGameStateBase_InGame>() : nullptr) 
    {
        WinsToFinish = GS->WinsToFinish; 
    }

    PaintTeamDots(Box, Wins, WinsToFinish); 
}

// 텍스트 도트로 그리기(필요하면 UImage로 바꿔도 됨)
void UTInGameHUD::PaintTeamDots(UHorizontalBox* Box, int32 Wins, int32 WinsToFinish)
{
    if (!Box) return;
    Box->ClearChildren();

    for (int32 i = 0; i < WinsToFinish; ++i)
    {
        UTextBlock* Dot = NewObject<UTextBlock>(Box); 
        const bool bFilled = (i < Wins);              
        Dot->SetText(FText::FromString(bFilled ? TEXT("●") : TEXT("○"))); 
        Dot->SetJustification(ETextJustify::Center); 

        // 글자 크기로 원 크기 조절
        FSlateFontInfo FontInfo = Dot->Font;
        FontInfo.Size = 64;              // 원하는 크기 (기본은 12~24 정도)
        Dot->SetFont(FontInfo);

        if (UHorizontalBoxSlot* DotSlot = Box->AddChildToHorizontalBox(Dot)) 
        {
            DotSlot->SetPadding(FMargin(2.f, 0.f)); 
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

    float Ratio = 0.f;

    if (APawn* P = GetOwningPlayerPawn())
    {
        // 1) GetStaminaRatio() 있으면 그것만 신뢰
        static FName FuncName = TEXT("GetStaminaRatio");
        if (UFunction* Fn = P->FindFunction(FuncName))
        {
            P->ProcessEvent(Fn, &Ratio);
        }
        else
        {
            // 2) (폴백) UPROPERTY로 공개된 Stamina/MaxStamina가 있을 경우만 사용
            float Cur = 0.f, Max = 0.f;
            const UClass* Cls = P->GetClass();
            if (FProperty* CurProp = Cls->FindPropertyByName(TEXT("Stamina")))
            {
                void* ObjPtr = CurProp->ContainerPtrToValuePtr<void>(P);
                Cur = CastFieldChecked<FFloatProperty>(CurProp)->GetFloatingPointPropertyValue(ObjPtr);
            }
            if (FProperty* MaxProp = Cls->FindPropertyByName(TEXT("MaxStamina")))
            {
                void* ObjPtr = MaxProp->ContainerPtrToValuePtr<void>(P);
                Max = CastFieldChecked<FFloatProperty>(MaxProp)->GetFloatingPointPropertyValue(ObjPtr);
            }
            Ratio = (Max > 0.f) ? (Cur / Max) : 0.f;
        }
    }

    StaminaBar->SetPercent(FMath::Clamp(Ratio, 0.f, 1.f)); //입력 무시, 값만 반영
}

void UTInGameHUD::NativeConstruct()
{
    Super::NativeConstruct();

    // GameState 델리게이트 바인딩
    if (APlayerController* PC = GetOwningPlayer())
    {
        if (UWorld* W = PC->GetWorld())
        {
            if (ATGameStateBase_InGame* GS = W->GetGameState<ATGameStateBase_InGame>())
            {
                GS->OnKillLog.AddDynamic(this, &UTInGameHUD::HandleKillLogToHUD); // ★ NEW
            }
        }
    }
}

void UTInGameHUD::HandleKillLogToHUD(const FKillLogEntry& Entry)   // ★ NEW
{
    AddKillLogTextLine(Entry);
}

FString UTInGameHUD::TeamToLabel(ETeam Team)                        // ★ NEW
{
    switch (Team)
    {
    case ETeam::Police: return TEXT("Police");
    case ETeam::Thief:  return TEXT("Thief");
    default:            return TEXT("Unknown");
    }
}

void UTInGameHUD::AddKillLogTextLine(const FKillLogEntry& Entry)   // ★ NEW
{
    if (!KillLogBox) return;

    // 라벨 만들기: Killer = Police/Thief/Unknown, Victim = Thief/Police/NPC
    const FString KillerLabel = TeamToLabel(Entry.KillerTeam);      // Police / Thief / Unknown
    const FString VictimLabel = Entry.bVictimIsAI
        ? TEXT("NPC")
        : TeamToLabel(Entry.VictimTeam);                            // Thief / Police / Unknown

    const FString Line = FString::Printf(TEXT("%s ➠ %s"), *KillerLabel, *VictimLabel);

    UTextBlock* TB = NewObject<UTextBlock>(this, UTextBlock::StaticClass());
    if (!TB) return;

    TB->SetText(FText::FromString(Line));
    TB->SetJustification(ETextJustify::Left);
    TB->SetAutoWrapText(false);
    TB->SetColorAndOpacity(FSlateColor(FLinearColor(0.808f, 0.776f, 0.518f, 1.0f))); //  킬로그 금색 적용

    // (선택) 색상 톤: Killer/ Victim 팀별로 강조하고 싶다면 여기서 SetColorAndOpacity
    // 예) Police 파란, Thief 빨강, NPC 회색 등
    // if (Entry.bVictimIsAI) { TB->SetColorAndOpacity(FSlateColor(FLinearColor(0.7f,0.7f,0.7f,1))); }

    // 최신 ↑ 위에 쌓기: 맨 앞에 삽입
    KillLogBox->AddChild(TB);
    KillLogBox->InsertChildAt(0, TB);

    // 최대 줄 수 넘으면 맨 아래(가장 오래된 것) 제거
    while (KillLogBox->GetChildrenCount() > MaxKillLines)
    {
        if (UWidget* Oldest = KillLogBox->GetChildAt(KillLogBox->GetChildrenCount() - 1))
        {
            Oldest->RemoveFromParent();
        }
    }

    // 일정 시간 뒤 자동 제거 (간단 버전)
    if (UWorld* W = GetWorld())
    {
        TWeakObjectPtr<UTextBlock> WeakTB = TB;
        FTimerHandle Tmp;
        W->GetTimerManager().SetTimer(
            Tmp,
            FTimerDelegate::CreateLambda([WeakTB]()
                {
                    if (WeakTB.IsValid())
                    {
                        if (UWidget* Widget = WeakTB.Get())
                        {
                            Widget->RemoveFromParent();
                        }
                    }
                }),
            LineLifeSeconds, false
        );
    }
}