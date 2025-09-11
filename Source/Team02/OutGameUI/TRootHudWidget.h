// TRootHudWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TRootHudWidget.generated.h"

UCLASS()
class TEAM02_API UTRootHudWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
	UPROPERTY(meta=(BindWidget))
	class ULobbyWidget* LobbyWidget = nullptr;
	
	UFUNCTION(BlueprintCallable)
	void RefreshLobby();
};
