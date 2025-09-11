// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "TGameStateBase_Lobby.generated.h"

class ATPlayerState;

UENUM(BlueprintType)
enum class EMatchPhase : uint8
{
	Waiting,
	Loading,
	InGame,
	RoundEnd,
	MatchEnd
};

DECLARE_MULTICAST_DELEGATE(FOnLobbyCountsChanged);

UCLASS()
class TEAM02_API ATGameStateBase_Lobby : public AGameStateBase
{
	GENERATED_BODY()

public:
	ATGameStateBase_Lobby();

	/** 현재 매치 페이즈 */
	UPROPERTY(ReplicatedUsing=OnRep_Phase, BlueprintReadOnly, Category="Lobby")
	EMatchPhase Phase = EMatchPhase::Waiting;

	/** 준비한 플레이어 수 / 전체 인원 */
	UPROPERTY(ReplicatedUsing=OnRep_Counts, BlueprintReadOnly, Category="Lobby")
	int32 ReadyCount = 0;

	UPROPERTY(ReplicatedUsing=OnRep_Counts, BlueprintReadOnly, Category="Lobby")
	int32 TotalPlayers = 0;

	/** (선택) 카운트다운 */
	UPROPERTY(Replicated, BlueprintReadOnly, Category="Lobby")
	int32 LobbyCountdown = 0; // 0이면 비활성

	// 합계 재계산(서버에서만 호출)
	void RecalcCounts();

	// 위젯/컨트롤러가 구독할 신호
	FOnLobbyCountsChanged OnLobbyCountsChanged;

	// 접근자
	UFUNCTION(BlueprintPure) int32 GetReadyCount() const { return ReadyCount; }
	UFUNCTION(BlueprintPure) int32 GetTotalPlayers() const { return TotalPlayers; }

	// 리플리케이션 등록
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// RepNotify
	UFUNCTION() void OnRep_Phase();
	UFUNCTION() void OnRep_Counts();

	// 플레이어 입/퇴장 시 합계 갱신을 위해 훅킹
	virtual void AddPlayerState(APlayerState* PS) override;
	virtual void RemovePlayerState(APlayerState* PS) override;
};