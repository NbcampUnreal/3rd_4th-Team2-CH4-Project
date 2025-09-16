// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "OutGameUI/TTeamTypes.h"
#include "TPlayerState_InGame.generated.h"

UCLASS()
class TEAM02_API ATPlayerState_InGame : public APlayerState
{
	GENERATED_BODY()

public:
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="Team")
	ETeam Team = ETeam::None;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
