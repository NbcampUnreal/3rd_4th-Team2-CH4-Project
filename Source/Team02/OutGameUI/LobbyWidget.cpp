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
	if (BtnTeam)  BtnTeam->OnClicked.AddDynamic(this, &ULobbyWidget::OnClickTeam);
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
	if (ATUPlayerController* PC = GetTUPlayerController())
	{
		if (ATPlayerState* TPS = PC->GetPlayerState<ATPlayerState>())
		{
			if (TxtTeam)
			{
				const TCHAR* TeamStr =
					(TPS->Team == ETeam::Police) ? TEXT("Police") :
					(TPS->Team == ETeam::Thief ) ? TEXT("Thief")  : TEXT("None");
				TxtTeam->SetText(FText::FromString(
					FString::Printf(TEXT("Team: %s"), TeamStr)));
			}
			if (TxtReady)
			{
				TxtReady->SetText(TPS->bReady
					? FText::FromString(TEXT("Ready: YES"))
					: FText::FromString(TEXT("Ready: NO")));
			}
		}
	}

	if (UWorld* W = GetWorld())
	{
		if (ATGameStateBase_Lobby* GS = W->GetGameState<ATGameStateBase_Lobby>())
		{
			// 합계 표시
			if (TxtTotals)
			{
				TxtTotals->SetText(FText::FromString(
					FString::Printf(TEXT("Ready %d / %d"), GS->GetReadyCount(), GS->GetTotalPlayers())));
			}

			if (TxtCountdown)  // UTextBlock* TxtCountdown 를 BP에 추가해야 함
			{
				TxtCountdown->SetText(FText::FromString(
					FString::Printf(TEXT("Match starts in: %d"), GS->LobbyCountdown)));
			}

			// 전체 목록 표시
			if (VBoxPlayers)
			{
				VBoxPlayers->ClearChildren();

				for (APlayerState* PS : GS->PlayerArray)
				{
					if (ATPlayerState* TPS = Cast<ATPlayerState>(PS))
					{
						const TCHAR* TeamStr =
							(TPS->Team == ETeam::Police) ? TEXT("Police") :
							(TPS->Team == ETeam::Thief ) ? TEXT("Thief")  : TEXT("None");

						const FString Line = FString::Printf(
							TEXT("%s   |   Team: %s   |   Ready: %s"),
							*PS->GetPlayerName(),
							TeamStr,
							TPS->bReady ? TEXT("YES") : TEXT("NO"));

						UTextBlock* Row = NewObject<UTextBlock>(this);
						Row->SetText(FText::FromString(Line));
						VBoxPlayers->AddChild(Row);
					}
				}
			}
		}
	}
}

void ULobbyWidget::OnClickTeam()
{
	if (ATUPlayerController* PC = GetTUPlayerController())
	{
		PC->Server_CycleTeam();
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