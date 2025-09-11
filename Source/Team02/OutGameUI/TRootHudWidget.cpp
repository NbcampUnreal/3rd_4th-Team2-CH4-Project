// Fill out your copyright notice in the Description page of Project Settings.


#include "TRootHudWidget.h"
#include "LobbyWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/TextBlock.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Blueprint/WidgetTree.h"

void UTRootHudWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (WidgetTree)
    {
        UOverlay* RootOverlay = Cast<UOverlay>(GetRootWidget());
        if (!RootOverlay)
        {
            RootOverlay = WidgetTree->ConstructWidget<UOverlay>(UOverlay::StaticClass());
            WidgetTree->RootWidget = RootOverlay;
        }

        if (LobbyWidget)
        {
            if (LobbyWidget->GetParent())
            {
                LobbyWidget->RemoveFromParent();
            }

            // 이름을 Slot → AddedSlot 으로 변경
            if (UOverlaySlot* AddedSlot = RootOverlay->AddChildToOverlay(LobbyWidget))
            {
                AddedSlot->SetHorizontalAlignment(HAlign_Fill);
                AddedSlot->SetVerticalAlignment(VAlign_Fill);
                AddedSlot->SetPadding(FMargin(0));
            }
        }
    }
    
    if (UOverlay* RootOverlay = Cast<UOverlay>(GetRootWidget()))
    {
        if (UTextBlock* Txt = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass()))
        {
            Txt->SetText(FText::FromString(TEXT("UI DEBUG")));
            Txt->SetColorAndOpacity(FSlateColor(FLinearColor::Yellow));
            if (UOverlaySlot* S = RootOverlay->AddChildToOverlay(Txt))
            {
                S->SetHorizontalAlignment(HAlign_Center);
                S->SetVerticalAlignment(VAlign_Top);
                S->SetPadding(FMargin(0, 20, 0, 0));
            }
        }
    }
    SetVisibility(ESlateVisibility::Visible);
    SetRenderOpacity(1.0f);
    
}

void UTRootHudWidget::RefreshLobby()
{
    if (LobbyWidget)
    {
        LobbyWidget->RefreshUI();
    }
}