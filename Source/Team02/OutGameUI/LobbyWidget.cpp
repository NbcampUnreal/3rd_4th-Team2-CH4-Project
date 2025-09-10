// Fill out your copyright notice in the Description page of Project Settings.


#include "OutGameUI/LobbyWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "TUPlayerController.h"
#include "TPlayerState.h"
#include "TTeamTypes.h"

void ULobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (BtnTeam)  BtnTeam->OnClicked.AddDynamic(this, &ULobbyWidget::OnClickTeam);
	if (BtnReady) BtnReady->OnClicked.AddDynamic(this, &ULobbyWidget::OnClickReady);
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
		PC->Server_ToggleReady();
	}
}

ATUPlayerController* ULobbyWidget::GetTUPlayerController() const
{
	return GetOwningPlayer() ? Cast<ATUPlayerController>(GetOwningPlayer()) : nullptr;
}