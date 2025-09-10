// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyWidget.generated.h"

class UButton; class UTextBlock;

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

	UFUNCTION(BlueprintCallable) void RefreshUI();

private:
	UFUNCTION() void OnClickTeam();
	UFUNCTION() void OnClickReady();
	class ATUPlayerController* GetTUPlayerController() const;
};
