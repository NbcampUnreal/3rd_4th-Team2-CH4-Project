// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "TGameStateBase_Lobby.generated.h"

UENUM(BlueprintType)
enum class EMatchPhase : uint8
{
	Waiting,
	Loading,
	InGame,
	RoundEnd,
	MatchEnd
};

UCLASS()
class TEAM02_API ATGameStateBase_Lobby : public AGameStateBase
{
	GENERATED_BODY()

public:
	// ★ 생성자 선언 추가
	ATGameStateBase_Lobby();

	// 복제될 변수들
	UPROPERTY(ReplicatedUsing=OnRep_Phase, BlueprintReadOnly, Category="Lobby")
	EMatchPhase Phase = EMatchPhase::Waiting;

	UPROPERTY(Replicated, BlueprintReadOnly, Category="Lobby")
	int32 ReadyCount = 0;

	UPROPERTY(Replicated, BlueprintReadOnly, Category="Lobby")
	int32 TotalPlayers = 0;

	UPROPERTY(Replicated, BlueprintReadOnly, Category="Lobby")
	int32 LobbyCountdown = 0; // 0이면 비활성

	// (권장) public으로 두는 게 안전
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& Out) const override;

protected:
	UFUNCTION()
	void OnRep_Phase();
};