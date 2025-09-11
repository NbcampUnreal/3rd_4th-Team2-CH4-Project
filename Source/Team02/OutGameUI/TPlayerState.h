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

	/* 팀/Ready는 서버에서만 수정, 클라 복제*/
	UPROPERTY(ReplicatedUsing=OnRep_Team, VisibleAnywhere, BlueprintReadOnly, Category = "Lobby")
	ETeam Team = ETeam::None;

	UPROPERTY(ReplicatedUsing=OnRep_Ready, VisibleAnywhere, BlueprintReadOnly, Category = "Lobby")
	bool bReady = false;
	
	/*서버 전용 변경 */
	void SetTeam(ETeam NewTeam);
	void SetReady(bool bInReady);

	FOnLobbyStatusChanged OnLobbyStatusChanged;


protected:
	/*클라 UI 갱신*/
	UFUNCTION() void OnRep_Team();
	UFUNCTION() void OnRep_Ready();

	UFUNCTION(Server, Reliable) void ServerSetTeam(ETeam NewTeam);
	UFUNCTION(Server, Reliable) void ServerSetReady(bool bInReady);

	// 실제 값 세팅(서버 권한에서만 실행)
	void ApplyTeam_Server(ETeam NewTeam);
	void ApplyReady_Server(bool bInReady);
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
