// TPlayerState.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "TTeamTypes.h"
#include "TPlayerState.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnLobbyStatusChanged);

UCLASS()
class TEAM02_API ATPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ATPlayerState();

	UPROPERTY(ReplicatedUsing=OnRep_Team, VisibleAnywhere, BlueprintReadOnly, Category = "Lobby")
	ETeam Team = ETeam::None;

	UPROPERTY(ReplicatedUsing=OnRep_Ready, VisibleAnywhere, BlueprintReadOnly, Category = "Lobby")
	bool bReady = false;

	void SetTeam(ETeam NewTeam);
	void SetReady(bool bInReady);

	FOnLobbyStatusChanged OnLobbyStatusChanged;
	virtual void CopyProperties(APlayerState* NewPlayerState) override;

protected:

	UFUNCTION() void OnRep_Team();
	UFUNCTION() void OnRep_Ready();

	UFUNCTION(Server, Reliable) void ServerSetTeam(ETeam NewTeam);
	UFUNCTION(Server, Reliable) void ServerSetReady(bool bInReady);
	
	void ApplyTeam_Server(ETeam NewTeam);
	void ApplyReady_Server(bool bInReady);
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
