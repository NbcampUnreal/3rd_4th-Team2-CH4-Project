// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameLevel/TGameModeBase_InGame.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/PlayerState.h"
#include "TPlayerState_InGame.h"
#include "EngineUtils.h"

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

        // 1) PlayerStart의 전용 태그 필드 우선
        if (Wanted != NAME_None && PS->PlayerStartTag == Wanted)
            return PS;

        // 2) (호환) Actor Tags에도 넣어뒀다면 이것도 허용
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