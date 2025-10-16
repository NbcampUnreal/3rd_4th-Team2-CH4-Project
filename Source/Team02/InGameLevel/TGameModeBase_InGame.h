// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "OutGameUI/TTeamTypes.h"
#include "TGameModeBase_InGame.generated.h"

class ATPlayerState_InGame;
/**
 * 인게임 규칙 집행:
 * - 2명 접속 시 라운드 시작
 * - 즉사/킬 보고 → 킬 로그 브로드캐스트 + 라운드 승리 처리
 * - 5판 3선승 충족 시 결과 처리
 */
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

public:

	UPROPERTY(EditDefaultsOnly, Category = "Rule")
	int32 RoundSeconds = 180; // 3분 (3~5분 권장)

	virtual void PostLogin(APlayerController* NewPlayer) override;

	//캐릭터가 킬/사망을 보고할 때 호출(서버) 
	UFUNCTION(BlueprintCallable) void HandleEliminated(AController* Killer, AController* Victim);

	void TryStartRoundIfReady();
	void EndRound(APlayerState* LastVictimPS);
	void EndMatchAndShowResult();

	//  시간 만료(Thief가 3분 버팀)
	UFUNCTION(BlueprintCallable, Category = "Round")
	void HandleRoundTimeOver(); 

protected:
	//  SeamlessTravel 이후에도 라운드 시작을 보장하기 위해 훅 추가
	virtual void BeginPlay() override;                 
	virtual void PostSeamlessTravel() override;       
	virtual void HandleSeamlessTravelPlayer(AController*& C) override; 

	//  심리스 직후 2인 조건 충족을 기다리는 짧은 폴링 타이머
	FTimerHandle SeamlessStartRoundRetryHandle;        
	int32 SeamlessStartRoundRetries = 0;               // (최대 횟수 제한)

	//  라운드 종료 → 잠시 후 리스폰 & 다음 라운드 시작
	void EndRoundByTeamWin(ETeam WinnerTeam);     
	void RespawnAllPlayers();                     
	void StartNextRound();                        // (타이머 콜백)

	FTimerHandle RoundResetTimerHandle;          
	UPROPERTY(EditDefaultsOnly, Category = "Round")
	float RoundResetDelay = 1.5f;                 // (잠깐 멈춘 느낌용)

	//  NPC 리스폰은 BP 훅으로 처리(프로젝트별 스폰 방식 다르므로)
	UFUNCTION(BlueprintImplementableEvent, Category = "Round")
	void OnRoundReset_BP();                       //(선택 구현: NPC 전체 리스폰)
};
