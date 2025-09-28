#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TTeamTypes.h" // ETeam
#include "InputActionValue.h" // Enhanced Input
#include "Kismet/GameplayStatics.h"
#include "InGameLevel/TGameStateBase_InGame.h" 
#include "TUPlayerController.generated.h"

class UUserWidget;
class UInputMappingContext;
class UInputAction;
class ULobbyWidget;
class UTInGameHUD;

UCLASS()
class TEAM02_API ATUPlayerController : public APlayerController
{
	GENERATED_BODY()


public:
	ATUPlayerController();

	virtual void PreClientTravel(const FString& PendingURL, ETravelType TravelType, bool bIsSeamlessTravel) override;

	// ===== 생명주기 =====
	virtual void BeginPlay() override; // 맵 진입 시 UI/모드 설정 (Title/Lobby 구분)
	virtual void OnPossess(APawn* InPawn) override; // 인게임 진입 시 GameOnly + 매핑 적용
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void SetupInputComponent() override; // 스킬 입력 바인딩


	// ===== UI 에셋 =====
	UPROPERTY(EditDefaultsOnly, Category = "UI|Title")
	TSubclassOf<UUserWidget> TitleWidgetClass; // 타이틀 위젯(선택)
	
	UPROPERTY(EditDefaultsOnly, Category = "UI|Lobby")
	TSubclassOf<ULobbyWidget> LobbyWidgetClass; // 로비 위젯
	
	UPROPERTY(EditDefaultsOnly, Category = "UI|InGame")
	TSubclassOf<UTInGameHUD> InGameHUDClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> ResultWidgetClass; // WBP_GameResult   new

	// ===== 인풋 매핑/액션 =====
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext = nullptr;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> MoveAction = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> LookAction = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> SprintAction = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> AttackAction = nullptr;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> Skill1Action = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> Skill2Action;



	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> Skill3Action;





	// ===== 타이틀: BP 버튼에서 호출 =====
	UFUNCTION(BlueprintCallable, Category = "Title")
	void RequestEnterLobby(); // 타이틀 → 로비 (서버/클라 자동 분기)


	UFUNCTION(BlueprintCallable, Category = "Title")
	void JoinServer(const FString& InIPAddress); // IP 입력 접속 (선택)


	UFUNCTION(Server, Reliable)
	void Server_RequestEnterLobby();


	// ===== 로비: BP 버튼에서 호출 =====
	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void ToggleReady(); // 레디 토글 → 서버로 전달


	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void CycleTeam(); // 팀 순환(클라에서 호출) → 서버 RPC 무인자 버전 사용


	UFUNCTION(Server, Reliable)
	void Server_CycleTeam();


	UFUNCTION(Server, Reliable)
	void Server_SetReady(bool bReadyDesired);


	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void RefreshLobbyFromPS(); // PS값 → UI 갱신 브릿지


	UFUNCTION(BlueprintImplementableEvent, Category = "Lobby")
	void BP_UpdateLobbyUI(ETeam Team, bool bReady); // BP에서 텍스트/아이콘 갱신(선택)

	UFUNCTION(Client, Reliable)
	void Client_ShowResult(bool bWin); // 게임종료 전환


private:
	UPROPERTY() TObjectPtr<UUserWidget>  TitleWidgetInstance = nullptr;
	UPROPERTY() TObjectPtr<ULobbyWidget> LobbyWidgetInstance = nullptr;

	// ===== 맵 경로 (BP에서 변경 가능) =====
	UPROPERTY(EditDefaultsOnly, Category = "Maps")
	FString LobbyMapPath = TEXT("/Game/Team02/OutGameUI/Map/LobbyMap");

	// ===== 유틸 메서드 선언 =====
	void ShowTitleUI();
	void ShowLobbyUI();
	void HideAllUI();

	bool IsTitleMap() const;
	bool IsLobbyMap() const;

	// (인게임에서 매핑 적용이 필요하면)
	void ApplyInGameInputMapping();

	// 팀 순환 헬퍼 (서버에서 NextTeam 계산할 때 사용)
	static ETeam NextTeamOf(ETeam Cur);

	// — 아래 4개는 스킬 로그까지 쓰려면 선언 (원치 않으면 생략 가능) —
	UFUNCTION() void HandleSkill1(const FInputActionValue& Value);
	UFUNCTION() void HandleSkill2(const FInputActionValue& Value);
	UFUNCTION(Server, Reliable) void Server_HandleSkill(int32 SkillIndex, float Magnitude);
	UFUNCTION(NetMulticast, Unreliable) void Multicast_SkillLog(int32 SkillIndex, const FString& Who, float Magnitude);

	// UI
	UPROPERTY() TObjectPtr<UTInGameHUD> InGameHUDInstance = nullptr;
	UFUNCTION() void OnMatchFinished_ShowResult(EInGameTeam WinnerTeam); //new
	UPROPERTY() bool bResultShown = false; //new
	
	void ShowInGameHUD();
	void HideInGameHUD();
};