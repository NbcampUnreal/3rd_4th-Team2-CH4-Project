#include "TUPlayerController.h"
#include "EngineUtils.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerState.h"
#include "Engine/World.h"
#include "Camera/CameraActor.h"


// Enhanced Input
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "InputAction.h"


// Lobby
#include "OutGameUI/LobbyWidget.h" // ULobbyWidget::RefreshUI()
#include "OutGameUI/TGameModeBase_Lobby.h" // RecountLobbyAndMaybeStart()
#include "OutGameUI/TGameStateBase_Lobby.h" // OnLobbyCountsChanged 델리게이트
#include "TPlayerState.h" // Team/bReady + OnRep/델리게이트

#include "InGameUI/TInGameHUD.h"
#include "InGameLevel/TGameStateBase_InGame.h"
#include "TimerManager.h"

ATUPlayerController::ATUPlayerController()
{
	bReplicates = true;
}


void ATUPlayerController::BeginPlay()
{
    Super::BeginPlay();
    if (!IsLocalController()) return;


    if (IsTitleMap())
    {
        ShowTitleUI();


        FInputModeUIOnly UIOnly;
        UIOnly.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        SetInputMode(UIOnly);
        bShowMouseCursor = true;
        UE_LOG(LogTemp, Warning, TEXT("[PC][BeginPlay] TitleMap: UIOnly, Cursor ON"));
    }
    else if (IsLobbyMap())
    {
        ShowLobbyUI();
        RefreshLobbyFromPS();


        // 로비 카메라(있다면 첫 번째로 스냅)
        for (TActorIterator<ACameraActor> It(GetWorld()); It; ++It)
        {
            SetViewTargetWithBlend(*It, 0.f);
            break;
        }


        // 로비는 UIOnly + 커서 ON
        FInputModeUIOnly UIOnly;
        UIOnly.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        SetInputMode(UIOnly);
        bShowMouseCursor = true;
        UE_LOG(LogTemp, Warning, TEXT("[PC][BeginPlay] LobbyMap: UIOnly, Cursor ON"));


        // 로비 상태 변경 시 UI 갱신 연결
        if (ATPlayerState* TPS = GetPlayerState<ATPlayerState>())
        {
            TPS->OnLobbyStatusChanged.AddUObject(this, &ATUPlayerController::RefreshLobbyFromPS);
        }
        if (auto* GS = GetWorld()->GetGameState<ATGameStateBase_Lobby>())
        {
            GS->OnLobbyCountsChanged.AddUObject(this, &ATUPlayerController::RefreshLobbyFromPS);
        }
    }
    else
    {
        HideAllUI();              // 타이틀/로비 UI 숨김
        ShowInGameHUD();          // 새로 추가
        FInputModeGameOnly GameOnly;
        SetInputMode(GameOnly);
        bShowMouseCursor       = false;
        bEnableClickEvents     = false;
        bEnableMouseOverEvents = false;

        UWidgetBlueprintLibrary::SetFocusToGameViewport();
        ApplyInGameInputMapping();
        UE_LOG(LogTemp, Warning, TEXT("[PC][BeginPlay] GameOnly + Mapping + InGameHUD"));
    }
}

// ===== UI 유틸 =====
void ATUPlayerController::ShowTitleUI()
{
    if (TitleWidgetClass && !TitleWidgetInstance)
    {
        TitleWidgetInstance = CreateWidget<UUserWidget>(this, TitleWidgetClass);
        if (TitleWidgetInstance)
        {
            TitleWidgetInstance->AddToPlayerScreen(10000);
            TitleWidgetInstance->SetVisibility(ESlateVisibility::Visible);
        }
    }
}

void ATUPlayerController::ShowLobbyUI()
{
    if (LobbyWidgetClass && !LobbyWidgetInstance)
    {
        LobbyWidgetInstance = CreateWidget<ULobbyWidget>(this, LobbyWidgetClass);
        if (LobbyWidgetInstance)
        {
            LobbyWidgetInstance->AddToPlayerScreen(10000);
            LobbyWidgetInstance->SetVisibility(ESlateVisibility::Visible);
        }
    }

    if (LobbyWidgetInstance)
    {
        LobbyWidgetInstance->RefreshUI();
    }
}

void ATUPlayerController::HideAllUI()
{
    if (TitleWidgetInstance)
    {
        TitleWidgetInstance->RemoveFromParent();
        TitleWidgetInstance = nullptr;
    }
    if (LobbyWidgetInstance)
    {
        LobbyWidgetInstance->RemoveFromParent();
        LobbyWidgetInstance = nullptr;
    }
}

// ===== 맵 판별 =====
bool ATUPlayerController::IsTitleMap() const
{
    const FString LevelName = UGameplayStatics::GetCurrentLevelName(this, true /*RemovePrefix*/);
    return LevelName.Contains(TEXT("Title"), ESearchCase::IgnoreCase);
}

bool ATUPlayerController::IsLobbyMap() const
{
    const FString LevelName = UGameplayStatics::GetCurrentLevelName(this, true /*RemovePrefix*/);
    return LevelName.Contains(TEXT("Lobby"), ESearchCase::IgnoreCase);
}

// ===== 인풋 매핑 (선택) =====
void ATUPlayerController::ApplyInGameInputMapping()
{
    if (ULocalPlayer* LP = GetLocalPlayer())
    {
        if (UEnhancedInputLocalPlayerSubsystem* Sub = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
        {
            Sub->ClearAllMappings();

            if (!DefaultMappingContext)
            {
                // 안전망: 하드코딩된 경로로 로드
                DefaultMappingContext = LoadObject<UInputMappingContext>(
                    nullptr,
                    TEXT("/Game/Team02/Input/CharacterIMC.CharacterIMC")  // 실제 경로 맞춰주세요!
                );
                UE_LOG(LogTemp, Warning, TEXT("[PC][Mapping] DefaultMappingContext was NULL -> Loaded from path"));
            }

            if (DefaultMappingContext)
            {
                Sub->AddMappingContext(DefaultMappingContext, 0);
                UE_LOG(LogTemp, Warning, TEXT("[PC][Mapping] DefaultMappingContext added: %s"), *DefaultMappingContext->GetName());
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("[PC][Mapping] DefaultMappingContext is STILL NULL!"));
            }
        }
    }
}

// ===== 팀 순환 헬퍼 =====
ETeam ATUPlayerController::NextTeamOf(ETeam Cur)
{
    switch (Cur)
    {
    case ETeam::None:   return ETeam::Police;
    case ETeam::Police: return ETeam::Thief;
    case ETeam::Thief:  return ETeam::Police; // 2팀 왕복
    default:            return ETeam::Police;
    }
}

// ===== (선택) 스킬 입력 & 네트워크 로그 =====
void ATUPlayerController::HandleSkill1(const FInputActionValue& Value)
{
    const float Mag = Value.Get<float>();
    UE_LOG(LogTemp, Warning, TEXT("[Skill][Client] Skill1 Triggered. Value=%.3f"), Mag);
    Server_HandleSkill(1, Mag);
}

void ATUPlayerController::HandleSkill2(const FInputActionValue& Value)
{
    const float Mag = Value.Get<float>();
    UE_LOG(LogTemp, Warning, TEXT("[Skill][Client] Skill2 Triggered. Value=%.3f"), Mag);
    Server_HandleSkill(2, Mag);
}

void ATUPlayerController::Server_HandleSkill_Implementation(int32 SkillIndex, float Magnitude)
{
    const FString Who = (PlayerState) ? PlayerState->GetPlayerName() : GetName();
    UE_LOG(LogTemp, Warning, TEXT("[Skill][Server] Skill%d by %s. Value=%.3f"), SkillIndex, *Who, Magnitude);
    Multicast_SkillLog(SkillIndex, Who, Magnitude);
}

void ATUPlayerController::Multicast_SkillLog_Implementation(int32 SkillIndex, const FString& Who, float Magnitude)
{
    UE_LOG(LogTemp, Warning, TEXT("[Skill][All] Skill%d by %s. Value=%.3f"), SkillIndex, *Who, Magnitude);
}

// ===== 입력 바인딩 =====
void ATUPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();
    
    UE_LOG(LogTemp, Warning, TEXT("[PC][SetupInput] (Info) Gameplay bindings are handled by ATCharacter."));
}

// ===== 인게임 진입 시 처리 =====

void ATUPlayerController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    if (!IsLocalController()) return;

    if (!IsTitleMap() && !IsLobbyMap()) // 인게임 맵
    {
        HideAllUI();
        ShowInGameHUD();
        
        // GameOnly + 커서/이벤트 OFF
        FInputModeGameOnly GameOnly;
        SetInputMode(GameOnly);
        bShowMouseCursor       = false;
        bEnableClickEvents     = false;
        bEnableMouseOverEvents = false;

        // 뷰포트 포커스 강제
        UWidgetBlueprintLibrary::SetFocusToGameViewport();

        // 혹시 모를 블로킹 해제
        SetIgnoreMoveInput(false);
        SetIgnoreLookInput(false);

        // 매핑 컨텍스트 적용 (안전망 포함)
        ApplyInGameInputMapping();

        UE_LOG(LogTemp, Warning, TEXT("[PC][OnPossess] GameOnly + Mapping + Focus applied"));
    }
}


void ATUPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    HideInGameHUD();
    HideAllUI();
    Super::EndPlay(EndPlayReason);
}

void ATUPlayerController::ShowInGameHUD()
{
    if (InGameHUDInstance || !InGameHUDClass) return;

    InGameHUDInstance = CreateWidget<UTInGameHUD>(this, InGameHUDClass);
    if (!InGameHUDInstance) return;

    InGameHUDInstance->AddToViewport(1000);

    // GameState 델리게이트 바인딩 (타이머/스코어/킬)
    if (auto GS = GetWorld()->GetGameState<ATGameStateBase_InGame>())
    {
        GS->OnTimerUpdated.AddDynamic(InGameHUDInstance, &UTInGameHUD::OnTimerUpdated);
        GS->OnScoreUpdated.AddDynamic(InGameHUDInstance, &UTInGameHUD::OnScoreUpdated);
        GS->OnKillEvent.AddDynamic(InGameHUDInstance, &UTInGameHUD::OnKillEvent);

        // 초기 스냅샷
        InGameHUDInstance->OnTimerUpdated(GS->RemainingSec);
        InGameHUDInstance->OnScoreUpdated(GS->ThiefWins, GS->PoliceWins);
    }

    // 스태미너 Pull 타이머(0.1s)
    FTimerHandle StaminaTick;
    GetWorld()->GetTimerManager().SetTimer(
        StaminaTick,
        FTimerDelegate::CreateWeakLambda(this, [this]()
        {
            if (InGameHUDInstance) InGameHUDInstance->RefreshStaminaBar();
        }),
        0.1f, true
    );
}

void ATUPlayerController::HideInGameHUD()
{
    if (InGameHUDInstance)
    {
        InGameHUDInstance->RemoveFromParent();
        InGameHUDInstance = nullptr;
    }
}
// ===== 타이틀 → 로비 전환 =====
void ATUPlayerController::RequestEnterLobby()
{
    const ENetMode NM = GetNetMode();

    // 전용/리슨 서버에서만 방 열기
    if (IsRunningDedicatedServer() || NM == NM_ListenServer)
    {
        const FString Url = TEXT("/Game/Team02/OutGameUI/Map/LobbyMap?listen");
        UE_LOG(LogTemp, Warning, TEXT("[TITLE] ServerTravel -> %s"), *Url);
        GetWorld()->ServerTravel(*Url);
        return;
    }

    // 이미 서버에 붙어있는 클라면 서버 RPC로만 요청
    if (NM == NM_Client)
    {
        UE_LOG(LogTemp, Warning, TEXT("[TITLE] RequestEnterLobby via server RPC"));
        Server_RequestEnterLobby();
        return;
    }

    // Standalone(서버 안 붙은 클라)은 먼저 JoinServer 필요
    UE_LOG(LogTemp, Warning, TEXT("[TITLE] Not connected. Call JoinServer(IP:Port) first."));
}

void ATUPlayerController::JoinServer(const FString& InIPAddress)
{
    if (InIPAddress.IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("[TITLE] IP empty. Not joining."));
        return;
    }
    UE_LOG(LogTemp, Warning, TEXT("[TITLE] ClientTravel -> %s"), *InIPAddress);
    ClientTravel(InIPAddress, ETravelType::TRAVEL_Absolute);
}

void ATUPlayerController::Server_RequestEnterLobby_Implementation()
{
    UE_LOG(LogTemp, Warning, TEXT("[TITLE] Server_RequestEnterLobby on server. World=%s"), *GetWorld()->GetMapName());

    if (GetWorld())
    {
        const FString TravelURL = LobbyMapPath + TEXT("?listen");
        UE_LOG(LogTemp, Warning, TEXT("[TITLE] ServerTravel -> %s"), *TravelURL);
        GetWorld()->ServerTravel(*TravelURL);
    }
}

// ===== 로비 버튼 처리 & 서버 RPC =====
void ATUPlayerController::ToggleReady()
{
    if (ATPlayerState* PS = GetPlayerState<ATPlayerState>())
    {
        const bool bNew = !PS->bReady;
        UE_LOG(LogTemp, Warning, TEXT("[Lobby][Client] ToggleReady -> %s"), bNew ? TEXT("TRUE") : TEXT("FALSE"));
        Server_SetReady(bNew);         // 낙관 갱신 없이 OnRep에서 갱신
    }
    else
    {
        Server_SetReady(true);
    }
}

void ATUPlayerController::CycleTeam()
{
    UE_LOG(LogTemp, Warning, TEXT("[Lobby][Client] CycleTeam request"));
    Server_CycleTeam(); // 무인자: 서버에서 다음 팀 계산
}

void ATUPlayerController::Server_CycleTeam_Implementation()
{
    if (ATPlayerState* PS = GetPlayerState<ATPlayerState>())
    {
        const ETeam Next = NextTeamOf(PS->Team);
        PS->SetTeam(Next); // SetTeam 내부에서 Replicated 변수 변경 + OnRep 호출 가정
        UE_LOG(LogTemp, Warning, TEXT("[Lobby][Server] CycleTeam -> %d (Player=%s)"),
            static_cast<int32>(Next), *PS->GetPlayerName());
    }
}

void ATUPlayerController::Server_SetReady_Implementation(bool bReadyDesired)
{
    if (ATPlayerState* PS = GetPlayerState<ATPlayerState>())
    {
        if (PS->bReady != bReadyDesired)
        {
            PS->SetReady(bReadyDesired); // SetReady 내부에서 Replicated 변수 변경 + OnRep 호출 가정
        }
        UE_LOG(LogTemp, Warning, TEXT("[Lobby][Server] SetReady -> %s (Player=%s)"),
            bReadyDesired ? TEXT("TRUE") : TEXT("FALSE"), *PS->GetPlayerName());
    }

    if (ATGameModeBase_Lobby* GM = GetWorld()->GetAuthGameMode<ATGameModeBase_Lobby>())
    {
        GM->RecountLobbyAndMaybeStart();
    }
}

// ===== 로비 상태 → UI 갱신 브릿지 =====
void ATUPlayerController::RefreshLobbyFromPS()
{
    if (!IsLocalController()) return;

    if (ATPlayerState* PS = GetPlayerState<ATPlayerState>())
    {
        BP_UpdateLobbyUI(PS->Team, PS->bReady); // 선택: BP에서 텍스트/아이콘 갱신
    }
    if (LobbyWidgetInstance)
    {
        LobbyWidgetInstance->RefreshUI();       // ULobbyWidget 구현 호출
    }
}

void ATUPlayerController::PreClientTravel(const FString& PendingURL, ETravelType TravelType, bool bIsSeamlessTravel)
{
    UE_LOG(LogTemp, Warning, TEXT("[CLIENT][PC] PreClientTravel URL=%s Type=%d Seamless=%d"),
        *PendingURL, (int)TravelType, bIsSeamlessTravel);

    Super::PreClientTravel(PendingURL, TravelType, bIsSeamlessTravel);
}