// Fill out your copyright notice in the Description page of Project Settings.


#include "OutGameUI/LobbyWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "TUPlayerController.h"
#include "TPlayerState.h"
#include "TTeamTypes.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/VerticalBox.h"
#include "TGameStateBase_Lobby.h"                
#include "Kismet/GameplayStatics.h"  

void ULobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (BtnJoinPolice) BtnJoinPolice->OnClicked.AddDynamic(this, &ULobbyWidget::OnClickJoinPolice);
	if (BtnJoinThief)  BtnJoinThief ->OnClicked.AddDynamic(this, &ULobbyWidget::OnClickJoinThief);
	if (BtnReady) BtnReady->OnClicked.AddDynamic(this, &ULobbyWidget::OnClickReady);

	if (HBoxButtons)
	{
		if (UCanvasPanelSlot* S = Cast<UCanvasPanelSlot>(HBoxButtons->Slot))
		{
			// 중앙 고정 (해상도 무관)
			S->SetAnchors(FAnchors(0.5f, 0.5f));       // 중앙 앵커
			S->SetAlignment(FVector2D(0.5f, 0.5f));    // 자신의 중심 기준
			S->SetAutoSize(true);                      // 내용물 크기만큼
			S->SetPosition(FVector2D(0.f, 0.f));       // 오프셋 0 (중앙)
			// S->SetSize(FVector2D(600,150));         // 필요하면 고정 크기 대신 사용
		}
	}
	
	if (UWorld* W = GetWorld())
	{
		if (ATGameStateBase_Lobby* GS = W->GetGameState<ATGameStateBase_Lobby>())
		{
			GS->OnLobbyCountsChanged.AddWeakLambda(this, [this]()
			{
				RefreshUI();
			});
		}
	}

	InvalidateLayoutAndVolatility();
	ForceLayoutPrepass();
	RefreshUI();
}

void ULobbyWidget::RefreshUI()
{
    ATUPlayerController* PC = GetTUPlayerController();
    ATPlayerState* MyPS = PC ? PC->GetPlayerState<ATPlayerState>() : nullptr;

    // 본인 상태 텍스트 (기존처럼)
    if (MyPS && TxtTeam && TxtReady)
    {
        const TCHAR* TeamStr = (MyPS->Team == ETeam::Police) ? TEXT("Police")
                             : (MyPS->Team == ETeam::Thief)  ? TEXT("Thief")
                                                             : TEXT("None");
        TxtReady->SetText(MyPS->bReady ? FText::FromString(TEXT("Ready: YES"))
                                       : FText::FromString(TEXT("Ready: NO")));
    }

    // 좌/우 컬럼 재빌드
    if (ListPolice) ListPolice->ClearChildren();
    if (ListThief ) ListThief ->ClearChildren();

    if (UWorld* W = GetWorld())
    {
        if (ATGameStateBase_Lobby* GS = W->GetGameState<ATGameStateBase_Lobby>())
        {
            // 합계/카운트다운 (기존)
            if (TxtTotals)
            {
                TxtTotals->SetText(FText::FromString(
                    FString::Printf(TEXT("Ready %d / %d"), GS->GetReadyCount(), GS->GetTotalPlayers())));
            }
        	
        	if (TxtCountdown)
        	{
        		if (GS->bCountdownActive)
        		{
        			TxtCountdown->SetVisibility(ESlateVisibility::Visible);
        			TxtCountdown->SetText(FText::AsNumber(GS->LobbyCountdown));
        		}
        		else
        		{
        			TxtCountdown->SetVisibility(ESlateVisibility::Collapsed);
        		}
        	}

            // 각 팀별로 라인 추가
            for (APlayerState* PSBase : GS->PlayerArray)
            {
                if (ATPlayerState* TPS = Cast<ATPlayerState>(PSBase))
                {
                    const bool bLocal = (TPS == MyPS);
                    const FString Nick = PSBase->GetPlayerName();
                    const FString Line = FString::Printf(TEXT("%s%s  %s"),
                        *Nick,
                        bLocal ? TEXT(" (You)") : TEXT(""),
                        TPS->bReady ? TEXT("[READY]") : TEXT(""));

                    UTextBlock* Row = NewObject<UTextBlock>(this);
                    Row->SetText(FText::FromString(Line));

                    if (TPS->Team == ETeam::Police)
                    {
                        if (ListPolice) ListPolice->AddChild(Row);
                    }
                    else if (TPS->Team == ETeam::Thief)
                    {
                        if (ListThief) ListThief->AddChild(Row);
                    }
                }
            }
        }
    }

    // 레디면 팀 이동 버튼 비활성화
    const bool bReady = MyPS ? MyPS->bReady : false;
    if (BtnJoinPolice) BtnJoinPolice->SetIsEnabled(!bReady);
    if (BtnJoinThief ) BtnJoinThief ->SetIsEnabled(!bReady);

    InvalidateLayoutAndVolatility();
    ForceLayoutPrepass();
}

void ULobbyWidget::OnClickJoinPolice()
{
	if (ATUPlayerController* PC = GetTUPlayerController())
	{
		PC->Server_SetTeam(ETeam::Police);
	}
}

void ULobbyWidget::OnClickJoinThief()
{
	if (ATUPlayerController* PC = GetTUPlayerController())
	{
		PC->Server_SetTeam(ETeam::Thief);
	}
}

void ULobbyWidget::OnClickReady()
{
	if (ATUPlayerController* PC = GetTUPlayerController())
	{
		PC->ToggleReady();
	}
}

ATUPlayerController* ULobbyWidget::GetTUPlayerController() const
{
	return GetOwningPlayer() ? Cast<ATUPlayerController>(GetOwningPlayer()) : nullptr;
}