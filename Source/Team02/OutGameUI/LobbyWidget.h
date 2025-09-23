// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyWidget.generated.h"

class UButton; class UTextBlock; class UHorizontalBox;
class UVerticalBox;

UCLASS()
class TEAM02_API ULobbyWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY(meta=(BindWidget)) UButton* BtnTeam = nullptr;
	UPROPERTY(meta=(BindWidget)) UButton* BtnReady = nullptr;

	UPROPERTY(meta=(BindWidget)) UTextBlock* TxtTeam = nullptr;
	UPROPERTY(meta=(BindWidget)) UTextBlock* TxtReady = nullptr;

	UPROPERTY(meta=(BindWidget)) UTextBlock* TxtTotals = nullptr;
	UPROPERTY(meta=(BindWidget)) UVerticalBox* VBoxPlayers = nullptr;

	UPROPERTY(meta=(BindWidget)) UTextBlock* TxtCountdown = nullptr; 
	
	UFUNCTION(BlueprintCallable) void RefreshUI();

	UPROPERTY(meta=(BindWidget)) class UHorizontalBox* HBoxButtons = nullptr;

private:
	UFUNCTION() void OnClickTeam();
	UFUNCTION() void OnClickReady();
	class ATUPlayerController* GetTUPlayerController() const;
};
