// Fill out your copyright notice in the Description page of Project Settings.


#include "TRootHudWidget.h"
#include "LobbyWidget.h"

void UTRootHudWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UTRootHudWidget::RefreshLobby()
{
	if (LobbyWidget) LobbyWidget->RefreshUI();
}
