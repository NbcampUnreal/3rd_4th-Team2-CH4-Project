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
	
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<ULobbyWidget> LobbyWidgetClass;
	
	UPROPERTY() TObjectPtr<ULobbyWidget> LobbyWidgetInstance = nullptr;
	
	UFUNCTION(Server, Reliable) void Server_CycleTeam();

	UFUNCTION(Server, Reliable) void Server_SetReady(bool bReadyDesired);
	void Server_SetReady_Implementation(bool bReadyDesired);

	UFUNCTION(BlueprintCallable, Category="Lobby")
	void ToggleReady();
	
	void RefreshLobbyFromPS();

private:
	ETeam GetNextTeam(ETeam Current) const;
};
