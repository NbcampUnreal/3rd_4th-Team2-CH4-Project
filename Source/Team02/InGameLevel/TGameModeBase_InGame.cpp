// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameLevel/TGameModeBase_InGame.h"
#include "InGameLevel/TGameStateBase_InGame.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/PlayerState.h"
#include "TPlayerState_InGame.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"

ATGameModeBase_InGame::ATGameModeBase_InGame()
{
    PlayerStateClass = ATPlayerState_InGame::StaticClass();
}

static ETeam GetTeam(AController* C)
{
    if (!C) return ETeam::None;
    if (const ATPlayerState_InGame* PS = C->GetPlayerState<ATPlayerState_InGame>())
        return PS->Team;
    return ETeam::None;
}

AActor* ATGameModeBase_InGame::ChoosePlayerStart_Implementation(AController* Player)
{
    const ETeam Team = GetTeam(Player); // PlayerState에서 팀 가져오는 헬퍼
    const FName Wanted = (Team == ETeam::Police) ? PoliceStartTag :
                         (Team == ETeam::Thief)  ? ThiefStartTag  : NAME_None;

    APlayerStart* Fallback = nullptr;

    for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
    {
        APlayerStart* PS = *It;
        if (!Fallback) Fallback = PS;
        
        if (Wanted != NAME_None && PS->PlayerStartTag == Wanted)
            return PS;
        
        if (Wanted != NAME_None && PS->Tags.Contains(Wanted))
            return PS;
    }

    UE_LOG(LogTemp, Warning, TEXT("[Spawn] WantedStartTag %s not found. Using fallback."), *Wanted.ToString());
    return Fallback ? Fallback : Super::ChoosePlayerStart_Implementation(Player);
}

APawn* ATGameModeBase_InGame::SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot)
{
    const ETeam Team = GetTeam(NewPlayer);
    TSubclassOf<APawn> UseClass =
        (Team == ETeam::Police) ? PolicePawnClass :
        (Team == ETeam::Thief)  ? ThiefPawnClass  : DefaultPawnClass;

    if (!UseClass)
    {
        UE_LOG(LogTemp, Error, TEXT("[Spawn] No PawnClass for Team %s"), *UEnum::GetValueAsString(Team));
        return nullptr;
    }

    FTransform SpawnTM = StartSpot ? StartSpot->GetTransform() : FTransform::Identity;
    FActorSpawnParameters Params;
    Params.Owner = NewPlayer;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    APawn* NewPawn = GetWorld()->SpawnActor<APawn>(UseClass, SpawnTM, Params);
    if (!NewPawn)
    {
        UE_LOG(LogTemp, Error, TEXT("[Spawn] Failed to spawn pawn for Team %s"), *UEnum::GetValueAsString(Team));
        return nullptr;
    }

    NewPlayer->Possess(NewPawn);
    return NewPawn;
}

void ATGameModeBase_InGame::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);
    TryStartRoundIfReady();
}

void ATGameModeBase_InGame::TryStartRoundIfReady()
{
    // 2인 게임: 두 명 이상 접속 시 라운드 시작
    if (GameState && GameState->PlayerArray.Num() >= 2)
    {
        if (auto GS = GetGameState<ATGameStateBase_InGame>())
        {
            GS->StartRound(RoundSeconds);
        }
    }
}

void ATGameModeBase_InGame::HandleEliminated(AController* Killer, AController* Victim)
{
    // HUD 킬 로그 전파
    if (auto GS = GetGameState<ATGameStateBase_InGame>())
    {
        const FString K = (Killer && Killer->PlayerState) ? Killer->PlayerState->GetPlayerName() : TEXT("Unknown");
        const FString V = (Victim && Victim->PlayerState) ? Victim->PlayerState->GetPlayerName() : TEXT("Unknown");
        GS->BroadcastKill(K, V);
    }

    EndRound(Victim ? Victim->PlayerState : nullptr);
}

void ATGameModeBase_InGame::EndRound(APlayerState* LastVictimPS)
{
    if (auto GS = GetGameState<ATGameStateBase_InGame>())
    {
        // ★ 간단판정: LastVictim이 'Police'면 도둑 승, 아니면 경찰 승
        //   (정확한 팀 판정은 PlayerState의 팀(ETeam) 값으로 교체 권장)
        const bool bPoliceDead = LastVictimPS && LastVictimPS->GetPlayerName().Contains(TEXT("Police"));
        GS->AddWin(bPoliceDead ? EInGameTeam::Thief : EInGameTeam::Police);

        const bool bFinish =
            (GS->ThiefWins >= GS->WinsToFinish) ||
            (GS->PoliceWins >= GS->WinsToFinish) ||
            (GS->CurrentRound > GS->MaxRounds);

        if (bFinish) EndMatchAndShowResult();
        else         GS->StartRound(RoundSeconds);
    }
}

void ATGameModeBase_InGame::EndMatchAndShowResult()
{
    // 결과 위젯(UTGameResult)을 띄우거나 결과 맵으로 이동하는 처리 권장.
    // 프로젝트에는 이미 결과 위젯에서 Title로 복귀하는 흐름이 있으니 재사용할 수 있음.
    // (로비→인게임 시나리오는 기존 코드가 서버 트래블/카운트다운으로 이어짐) :contentReference[oaicite:6]{index=6}
}