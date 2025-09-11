// TUPlayerController.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TTeamTypes.h"
#include "TUPlayerController.generated.h"

class ULobbyWidget;

UCLASS()
class TEAM02_API ATUPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ATUPlayerController();
	virtual void BeginPlay() override;

	/** 바로 띄울 로비 위젯 BP 클래스 (WBP_LobbyWidget 지정) */
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<ULobbyWidget> LobbyWidgetClass;

	/** 런타임 인스턴스 */
	UPROPERTY() TObjectPtr<ULobbyWidget> LobbyWidgetInstance = nullptr;

	/** ---- 클라→서버: 로비 버튼 입력 ---- */
	UFUNCTION(Server, Reliable) void Server_CycleTeam();
	UFUNCTION(Server, Reliable) void Server_ToggleReady();

	/** PS OnRep에서 호출: 로비 UI 갱신 */
	void RefreshLobbyFromPS();

private:
	ETeam GetNextTeam(ETeam Current) const;
};
