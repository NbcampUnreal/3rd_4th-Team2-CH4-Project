// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameLevel/TGameModeBase_InGame.h"
#include "InGameLevel/TGameStateBase_InGame.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/PlayerState.h"
#include "TPlayerState_InGame.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "OutGameUI/TTeamTypes.h"
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
    const ETeam Team = (Player && Player->PlayerState)
        ? (Cast<ATPlayerState_InGame>(Player->PlayerState) ? Cast<ATPlayerState_InGame>(Player->PlayerState)->Team : ETeam::None)
        : ETeam::None;

    const FName WantedTag = (Team == ETeam::Police) ? PoliceStartTag :
        (Team == ETeam::Thief) ? ThiefStartTag : NAME_None;

    APlayerStart* AnyStart = nullptr;            
    APlayerStart* TaggedStart = nullptr;         

    for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It) 
    {
        APlayerStart* PS = *It;
        if (!AnyStart) AnyStart = PS;           // 처음 아무거나 기억
        if (WantedTag != NAME_None && PS->PlayerStartTag == WantedTag)
        {
            TaggedStart = PS;                   // 태그 일치 우선 
            break;                              
        }
    }

    if (TaggedStart)
    {
        UE_LOG(LogTemp, Warning, TEXT("[ChooseStart] %s by tag %s"),
            *GetNameSafe(TaggedStart), *WantedTag.ToString()); 
        return TaggedStart;                                        
    }

    if (AnyStart)
    {
        UE_LOG(LogTemp, Warning, TEXT("[ChooseStart] Fallback to first start: %s"),
            *GetNameSafe(AnyStart));                           
        return AnyStart;                                           
    }

    UE_LOG(LogTemp, Error, TEXT("[ChooseStart] No PlayerStart in map. Using Super fallback")); 
    return Super::ChoosePlayerStart_Implementation(Player);        
}

APawn* ATGameModeBase_InGame::SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot)
{
    const ETeam Team = GetTeam(NewPlayer);
    TSubclassOf<APawn> UseClass =
        (Team == ETeam::Police) ? PolicePawnClass :
        (Team == ETeam::Thief)  ? ThiefPawnClass  : DefaultPawnClass;

    // 팀 전용 PawnClass가 비어있으면 DefaultPawnClass로 폴백
    if (!UseClass) 
    {              
        UE_LOG(LogTemp, Warning, TEXT("[Spawn] Team %s has no specific PawnClass. Fallback to DefaultPawnClass=%s"),
            *UEnum::GetValueAsString(Team), *GetNameSafe(DefaultPawnClass));                                          
        UseClass = DefaultPawnClass;                                                                                   
    }

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

    //NewPlayer->Possess(NewPawn);
    
     //   SeamlessTravel 후 자동 포제션이 누락되는 케이스 대비, 명시적 포제션 보장
    if (NewPawn && NewPlayer->GetPawn() != NewPawn) NewPlayer->Possess(NewPawn);
    return NewPawn;
}

void ATGameModeBase_InGame::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);
    TryStartRoundIfReady();
}

void ATGameModeBase_InGame::TryStartRoundIfReady()
{
    //  서버에서만 체크
    if (!HasAuthority()) 
    {                    
        UE_LOG(LogTemp, Warning, TEXT("[InGameGM] TryStartRoundIfReady: NOT Authority")); 
        return;          
    }                    

    //  GameState 유효성 및 클래스명 출력
    if (!GameState) 
    {               
        UE_LOG(LogTemp, Warning, TEXT("[InGameGM] TryStartRoundIfReady: GameState is NULL")); 
        return;     
    }               
    UE_LOG(LogTemp, Warning, TEXT("[InGameGM] TryStartRoundIfReady: GS=%s"), *GetNameSafe(GameState)); 
    UE_LOG(LogTemp, Warning, TEXT("[InGameGM] TryStartRoundIfReady: GSClass=%s"), *GameState->GetClass()->GetName()); 

    //  현재 PlayerArray 나열
    const int32 ArrNum = GameState->PlayerArray.Num(); 
    UE_LOG(LogTemp, Warning, TEXT("[InGameGM] TryStartRoundIfReady: PlayerArray.Num()=%d"), ArrNum); 
    for (int32 i = 0; i < ArrNum; ++i) 
    {                               
        APlayerState* PS = GameState->PlayerArray[i]; 
        UE_LOG(LogTemp, Warning, TEXT("[InGameGM]   [%d] %s (%s)"), i, *GetNameSafe(PS), PS ? *PS->GetClass()->GetName() : TEXT("NULL")); 
    }                               

    // 2인 게임: 두 명 이상 접속 시 라운드 시작
    if (GameState && GameState->PlayerArray.Num() >= 2)
    {
        if (auto GS = GetGameState<ATGameStateBase_InGame>())
        {
            GS->StartRound(RoundSeconds);

            //  시작에 성공했다면 폴링 타이머를 정리 (중복 호출 예방)
            if (SeamlessStartRoundRetryHandle.IsValid()) 
            {                                            
                GetWorldTimerManager().ClearTimer(SeamlessStartRoundRetryHandle); 
            }                                            
            return;
        }
        else
        {
            //  클래스 미스매치 가시화 (BP가 InGameState로 안 바뀐 케이스 등)
            UE_LOG(LogTemp, Error, TEXT("[InGameGM] PlayerArray>=2 BUT GameState is NOT ATGameStateBase_InGame (actual: %s)"),
                *GameState->GetClass()->GetName()); 
        }
    }
    else
    {
        //  아직 2명 안됨 (Seamless 직후 흔함)
        UE_LOG(LogTemp, Warning, TEXT("[InGameGM] Not enough players yet (need 2).")); 
    }
}

void ATGameModeBase_InGame::HandleEliminated(AController* Killer, AController* Victim)
{
     //1. HUD 킬 로그 전파 (플레이어명 없으면 Unknown)
    if (ATGameStateBase_InGame* GS_String = GetGameState<ATGameStateBase_InGame>())
    {
        const FString K = (Killer && Killer->PlayerState) ? Killer->PlayerState->GetPlayerName() : TEXT("Unknown");
        const FString V = (Victim && Victim->PlayerState) ? Victim->PlayerState->GetPlayerName() : TEXT("NPC");
        //GS->BroadcastKill(K, V);
    }

    // ---- 킬로그 브로드캐스트 ----                                          // ★ NEW
    if (ATGameStateBase_InGame* GS = GetGameState<ATGameStateBase_InGame>())  // ★ NEW
    {
        FKillLogEntry Entry;                                                  // ★ NEW

        // Killer 팀
        if (Killer && Killer->PlayerState)
        {
            if (const auto* KPS = Cast<ATPlayerState_InGame>(Killer->PlayerState))
            {
                Entry.KillerTeam = KPS->Team;                                 // ★ NEW
            }
            else
            {
                Entry.KillerTeam = ETeam::None;                               // ★ NEW
            }
        }
        else
        {
            Entry.KillerTeam = ETeam::None;                                   // ★ NEW
        }

        // Victim 팀 / AI 여부
        if (Victim && Victim->PlayerState)
        {
            if (const auto* VPS = Cast<ATPlayerState_InGame>(Victim->PlayerState))
            {
                Entry.VictimTeam = VPS->Team;                                 // ★ NEW
            }
            else
            {
                Entry.VictimTeam = ETeam::None;                               // ★ NEW
            }
            Entry.bVictimIsAI = false;                                        // ★ NEW
        }
        else
        {
            // 컨트롤러/PlayerState가 없으면 NPC로 본다
            Entry.VictimTeam = ETeam::None;                                   // ★ NEW
            Entry.bVictimIsAI = true;                                         // ★ NEW
        }

        Entry.ServerTime = GetWorld()->GetTimeSeconds();                      // ★ NEW

        GS->MulticastKillLog(Entry);                                          // ★ NEW
    }

    // 2. AI(컨트롤러에 PlayerState 없음) 처치 시에는 라운드 종료하지 않음                              
    const bool bVictimIsPlayer = (Victim && Victim->IsPlayerController())                            
        || (Victim && Victim->PlayerState != nullptr);                                               
    if (!bVictimIsPlayer)                                                                            
    {                                                                                               
        return; // 킬로그만 남기고 종료 안 함                                                        
    }

    // (3) 플레이어가 죽었을 때, 어느 팀이 죽었는지로 승자 결정
    ETeam DeadTeam = ETeam::None;
    if (Victim && Victim->PlayerState)
    {
        if (const ATPlayerState_InGame* IPS = Cast<ATPlayerState_InGame>(Victim->PlayerState)) 
        {
            DeadTeam = IPS->Team; // Thief or Police                                          
        }
    }

    // 규칙: Thief(적 플레이어)가 죽으면 '경찰 승'
    //       경찰(플레이어)이 죽으면 'Thief 승'
    ETeam WinnerTeam = (DeadTeam == ETeam::Thief) ? ETeam::Police :
        (DeadTeam == ETeam::Police) ? ETeam::Thief : ETeam::None;             


   // EndRound(Victim ? Victim->PlayerState : nullptr);
    EndRoundByTeamWin(WinnerTeam);
}

// 시간 만료(Thief 생존 승) 
void ATGameModeBase_InGame::HandleRoundTimeOver()                                           
{
    // 3분 버티면 Thief 승리
    EndRoundByTeamWin(ETeam::Thief);                                                        
}

// 라운드 종료 처리 → 점수 반영 & 잠깐 멈춤 후 리스폰/다음 라운드 

void ATGameModeBase_InGame::EndRoundByTeamWin(ETeam WinnerTeam)                           
{
    if (ATGameStateBase_InGame* GS = GetGameState<ATGameStateBase_InGame>())
    {
        if (WinnerTeam == ETeam::Thief)
        {
            GS->AddWin(EInGameTeam::Thief); // ● Thief 점수 +1                            
        }
        else if (WinnerTeam == ETeam::Police)
        {
            GS->AddWin(EInGameTeam::Police); // ● 경찰 점수 +1                              
        }

        // 매치 종료 여부 판단(기존 규칙 준수: 3선승 등)
        const bool bFinished =
            (GS->ThiefWins >= GS->WinsToFinish) ||
            (GS->PoliceWins >= GS->WinsToFinish) ||
            (GS->CurrentRound >= GS->MaxRounds); // 프로젝트 로직에 맞게 유지               

        if (bFinished)
        {
           // EndMatchAndShowResult();  단순 호출 → 실제 종료 처리로 변경 
            const EInGameTeam Winner = (GS->ThiefWins >= GS->WinsToFinish) ? EInGameTeam::Thief : EInGameTeam::Police; 
            GS->FinishMatch(Winner);                                      
            EndMatchAndShowResult();
            return;
        }
    }

    // 잠깐 멈춘 느낌 → RoundResetDelay 후 전원 리스폰 + 다음 라운드 시작
    GetWorldTimerManager().ClearTimer(RoundResetTimerHandle);                               
    GetWorldTimerManager().SetTimer(
        RoundResetTimerHandle,
        this, &ATGameModeBase_InGame::StartNextRound,
        RoundResetDelay, false                                                                
    );
}

// 전원 리스폰 (플레이어/AI 공통 가능한 범위에서 처리) 

void ATGameModeBase_InGame::RespawnAllPlayers()                                              
{
    // 현재 컨트롤러들의 Pawn 제거 후, RestartPlayer 호출
    UWorld* World = GetWorld();
    if (!World) return;

    int32 Restarted = 0;                                                                

    // npc 부활 이상해서 삭제처리함 
    // GameMode 포인터 받아둠
    //AGameModeBase* GMBase = UGameplayStatics::GetGameMode(World);                

    // Pawn 파괴 → RestartPlayer에서 새 Pawn을 스폰/포제션
    //for (FConstControllerIterator It = World->GetControllerIterator(); It; ++It)            
    //{
    //    AController* C = It->Get();
    //    if (!C) continue;

    //    if (APawn* P = C->GetPawn())
    //    {
    //        C->UnPossess();                                                //(포제션 끊고)
    //        P->Destroy(); // 안전하게 제거                                                   
    //    }
    //    // PlayerController 뿐 아니라 AIController도 RestartPlayer 지원(스폰 규칙에 따름)
    //    RestartPlayer(C);                                                   
    //    ++Restarted;
    //}

     // “플레이어 컨트롤러만” 리스폰
    for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
    {                                                                                       
        APlayerController* PC = It->Get();                                                   
        if (!PC) continue;                                                                   

        if (APawn* P = PC->GetPawn())                                                        
        {                                                                                    
            PC->UnPossess();                                                                 
            P->Destroy();                                                                     
        }                                                                                    

        RestartPlayer(PC);                                                                   
        ++Restarted;                                                                         
    }

    UE_LOG(LogTemp, Warning, TEXT("[Respawn] Restarted Controllers: %d"), Restarted);  
    // NPC(프로젝트별 스폰 방식 상이) → BP 훅으로 처리
    OnRoundReset_BP(); // 블루프린트에서 NPC 스폰/리셋 구현하면 여기서 함께 처리됨            
}

// 딜레이 콜백: 전원 리스폰 후 새 라운드 시작 

void ATGameModeBase_InGame::StartNextRound()                                                 
{
    // 잠깐 멈춘 느낌: 전역 딜레이 ↓ (원하면 0.0~0.05 사이 값)      
    UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.05f);          

    RespawnAllPlayers();                                                                      

    // 멈춘 뒤 0.1초만에 다시 정상 속도로 복귀 (연출)             
    FTimerHandle UnpauseHandle;                                          
    GetWorldTimerManager().SetTimer(UnpauseHandle, FTimerDelegate::CreateWeakLambda(this, [this]() 
        {                                                                   
            UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);       
            if (ATGameStateBase_InGame* GS = GetGameState<ATGameStateBase_InGame>())
            {
                GS->StartRound(RoundSeconds);
            }
        }), 0.1f, false);                                                    
}

void ATGameModeBase_InGame::EndRound(APlayerState* LastVictimPS)
{
    if (auto GS = GetGameState<ATGameStateBase_InGame>())
    {
        //  간단판정: LastVictim이 'Police'면 도둑 승, 아니면 경찰 승
        //   (정확한 팀 판정은 PlayerState의 팀(ETeam) 값으로 교체 권장)
        const bool bPoliceDead = LastVictimPS && LastVictimPS->GetPlayerName().Contains(TEXT("Police"));
        GS->AddWin(bPoliceDead ? EInGameTeam::Thief : EInGameTeam::Police);

        const bool bFinish =
            (GS->ThiefWins >= GS->WinsToFinish) ||
            (GS->PoliceWins >= GS->WinsToFinish) ||
            (GS->CurrentRound > GS->MaxRounds);

        if (bFinish) //EndMatchAndShowResult(); 단순 호출 → 실제 종료 처리로 변경
        {
            const EInGameTeam Winner = (GS->ThiefWins >= GS->WinsToFinish) ? EInGameTeam::Thief : EInGameTeam::Police; 
            GS->FinishMatch(Winner);                                      
            EndMatchAndShowResult();                                      // (UI/후처리 진입점)
        }
        else
        {
            //GS->StartRound(RoundSeconds); //즉시시작 금지
            UE_LOG(LogTemp, Warning, TEXT("[RoundEnd] schedule respawn after %.2fs"), RoundResetDelay); 
            GetWorldTimerManager().ClearTimer(RoundResetTimerHandle);                
            GetWorldTimerManager().SetTimer(                                         
                RoundResetTimerHandle, this, &ThisClass::StartNextRound,
                RoundResetDelay, false
            );
        }
    }
}

void ATGameModeBase_InGame::EndMatchAndShowResult()
{
    // 결과 위젯(UTGameResult)을 띄우거나 결과 맵으로 이동하는 처리 권장.
    // 프로젝트에는 이미 결과 위젯에서 Title로 복귀하는 흐름이 있으니 재사용할 수 있음.
    // (로비→인게임 시나리오는 기존 코드가 서버 트래블/카운트다운으로 이어짐) :contentReference[oaicite:6]{index=6}
}

//  인게임 맵이 로드된 "서버"에서 BeginPlay 시에도 라운드 시도
void ATGameModeBase_InGame::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Warning, TEXT("[InGameGM] BeginPlay - TryStartRoundIfReady()"));
    TryStartRoundIfReady(); // SeamlessTravel로 들어와도 첫 틱에서 한 번 시도
}

//  SeamlessTravel 직후에도 한 번 더 라운드 시도
void ATGameModeBase_InGame::PostSeamlessTravel()
{
    Super::PostSeamlessTravel();
    UE_LOG(LogTemp, Warning, TEXT("[InGameGM] PostSeamlessTravel - TryStartRoundIfReady()"));
    TryStartRoundIfReady();

    //  0.25초 간격으로 최대 5초(=20회) 재시도 → 2명 되는 순간 StartRound 진입
    SeamlessStartRoundRetries = 0;                                                             
    GetWorldTimerManager().SetTimer(                                                           
        SeamlessStartRoundRetryHandle,                                                        
        FTimerDelegate::CreateWeakLambda(this, [this]()                                      
            {                                                                                    
                ++SeamlessStartRoundRetries;                                                    
                UE_LOG(LogTemp, Warning, TEXT("[InGameGM] Retry #%d - TryStartRoundIfReady()"), 
                    SeamlessStartRoundRetries);                                                  
                TryStartRoundIfReady();                                                         

                if (GameState && GameState->PlayerArray.Num() >= 2)                             
                {                                                                                
                    GetWorldTimerManager().ClearTimer(SeamlessStartRoundRetryHandle);           
                    return;                                                                     
                }                                                                               

                if (SeamlessStartRoundRetries >= 20)                                             
                {                                                                               
                    UE_LOG(LogTemp, Error, TEXT("[InGameGM] Retry timeout: players still < 2")); 
                    GetWorldTimerManager().ClearTimer(SeamlessStartRoundRetryHandle);           
                }                                                                                
            }),                                                                                  
        0.25f, true                                                                          
    );
}

//  플레이어가 SeamlessTravel로 넘어올 때마다 호출됨(서버)
//         모든 플레이어가 넘어오기 전에 BeginPlay가 먼저 불릴 수 있으므로,
//         여기서도 체크를 한 번 더 해 준다.
void ATGameModeBase_InGame::HandleSeamlessTravelPlayer(AController*& C)
{
    Super::HandleSeamlessTravelPlayer(C);
    UE_LOG(LogTemp, Warning, TEXT("[InGameGM] HandleSeamlessTravelPlayer - %s"), *GetNameSafe(C));
    TryStartRoundIfReady();
}