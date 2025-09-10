// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TGameModeBase_Lobby.generated.h"

class ATPlayerState;
class ATUPlayerController;
class ATGameStateBase_Lobby;
/**
 * 
 */
UCLASS()
class TEAM02_API ATGameModeBase_Lobby : public AGameModeBase
{
	GENERATED_BODY()

public:
	ATGameModeBase_Lobby();

	/** 최소 시작 인원(2인 기본) - 필요시 에디터에서 조정 */
	UPROPERTY(EditAnywhere, Category="Lobby")
	int32 MinPlayersToStart = 2;

	/** 전원 Ready면 곧바로 시작할지(=자동 시작) 여부 */
	UPROPERTY(EditAnywhere, Category="Lobby")
	bool bAutoStartWhenAllReady = false;

	// --- AGameModeBase 인터페이스 ---
	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

	/** (선택) 외부에서 Ready 토글 이후 호출해도 되는 공개 함수 */
	UFUNCTION(BlueprintCallable, Category="Lobby")
	void RecountLobbyAndMaybeStart();

protected:
	/** 로비 인원/레디 수를 다시 세고 GameState_Lobby에 반영 */
	void UpdateLobbyCounts();

	/** 전원 Ready + 최소 인원 충족 여부 */
	bool AreAllPlayersReady(int32& OutReady, int32& OutTotal) const;

	/** 전원 준비시 시작 로직(다음 단계에서 ServerTravel 연결) */
	void TryStartMatchIfReady();
};
