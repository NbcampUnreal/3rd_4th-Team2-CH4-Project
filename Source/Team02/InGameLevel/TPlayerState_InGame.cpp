// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameLevel/TPlayerState_InGame.h"
#include "Net/UnrealNetwork.h"

void ATPlayerState_InGame::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATPlayerState_InGame, Team);
}	