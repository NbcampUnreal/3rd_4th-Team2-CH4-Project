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
	Countdown,  
	Traveling,
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
	
	UPROPERTY(ReplicatedUsing=OnRep_Phase, BlueprintReadOnly, Category="Lobby")
	EMatchPhase Phase = EMatchPhase::Waiting;
	
	UPROPERTY(ReplicatedUsing=OnRep_Counts, BlueprintReadOnly, Category="Lobby")
	int32 ReadyCount = 0;

	UPROPERTY(ReplicatedUsing=OnRep_Counts, BlueprintReadOnly, Category="Lobby")
	int32 TotalPlayers = 0;
	
	UPROPERTY(Replicated, BlueprintReadOnly, Category="Lobby")
	int32 LobbyCountdown = 0;
	
	void RecalcCounts();
	
	FOnLobbyCountsChanged OnLobbyCountsChanged;
	
	UFUNCTION(BlueprintPure) int32 GetReadyCount() const { return ReadyCount; }
	UFUNCTION(BlueprintPure) int32 GetTotalPlayers() const { return TotalPlayers; }
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	void StartCountdown(int32 Seconds);
	
	void CancelCountdown();

protected:
	UFUNCTION()
	void OnRep_LobbyCountdown();
	
	void TickCountdown();
	
	FTimerHandle CountdownHandle;
	
	void NotifyCountdownFinished();
	
	UFUNCTION() void OnRep_Phase();
	UFUNCTION() void OnRep_Counts();
	
	virtual void AddPlayerState(APlayerState* PS) override;
	virtual void RemovePlayerState(APlayerState* PS) override;
};