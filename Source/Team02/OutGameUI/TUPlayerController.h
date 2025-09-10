// TUPlayerController.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TTeamTypes.h"
#include "TUPlayerController.generated.h"

class UTRootHudWidget;

UCLASS()
class TEAM02_API ATUPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ATUPlayerController();
	virtual void BeginPlay() override;

	/** 루트 HUD BP 클래스 할당 */
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UTRootHudWidget> RootHUDClass;

	UPROPERTY() TObjectPtr<UTRootHudWidget> RootHUD = nullptr;

	/** ---- 클라→서버: 로비 버튼 입력 ---- */
	UFUNCTION(Server, Reliable) void Server_CycleTeam();
	UFUNCTION(Server, Reliable) void Server_ToggleReady();

	/** PS OnRep에서 호출: 로비 UI 갱신 */
	void RefreshLobbyFromPS();

private:
	ETeam GetNextTeam(ETeam Current) const;
};
