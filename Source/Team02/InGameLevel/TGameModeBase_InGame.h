// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TGameModeBase_InGame.generated.h"

class ATPlayerState_InGame;

UCLASS()
class TEAM02_API ATGameModeBase_InGame : public AGameModeBase
{
	GENERATED_BODY()

public:
	ATGameModeBase_InGame();

	// 팀별 Pawn 클래스 지정(에디터에서 세팅 가능)
	UPROPERTY(EditDefaultsOnly, Category="Spawn")
	TSubclassOf<APawn> PolicePawnClass;

	UPROPERTY(EditDefaultsOnly, Category="Spawn")
	TSubclassOf<APawn> ThiefPawnClass;

	// 팀별 PlayerStart 태그 (Police / Thief)
	UPROPERTY(EditDefaultsOnly, Category="Spawn")
	FName PoliceStartTag = TEXT("Police");

	UPROPERTY(EditDefaultsOnly, Category="Spawn")
	FName ThiefStartTag = TEXT("Thief");

	// 스폰 지점 선택
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	// 팀별 Pawn 스폰
	virtual APawn* SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot) override;
};
