// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TGameModeBase_Lobby.generated.h"

class ATPlayerState;
class ATUPlayerController;
class ATGameStateBase_Lobby;

UCLASS()
class TEAM02_API ATGameModeBase_Lobby : public AGameModeBase
{
	GENERATED_BODY()

public:
	ATGameModeBase_Lobby();
	
	UPROPERTY(EditAnywhere, Category="Lobby")
	int32 MinPlayersToStart = 2;
	
	UPROPERTY(EditAnywhere, Category="Lobby")
	bool bAutoStartWhenAllReady = false;

	UPROPERTY(EditDefaultsOnly, Category="Lobby")
	FString NextMap = TEXT("/Game/Team02/Maps/UtopianCity/Maps/UtopianCityDemoMap");
	
	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;
	
	UFUNCTION(BlueprintCallable, Category="Lobby")
	void RecountLobbyAndMaybeStart();

	UFUNCTION(BlueprintCallable, Category="Lobby")
	void SetPlayerReady(APlayerController* PC, bool bReady);
	
	UFUNCTION()
	void StartMatchTravel();
	
protected:
	void UpdateLobbyCounts();
	
	bool AreAllPlayersReady(int32& OutReady, int32& OutTotal) const;
	
	void TryStartMatchIfReady();
};
