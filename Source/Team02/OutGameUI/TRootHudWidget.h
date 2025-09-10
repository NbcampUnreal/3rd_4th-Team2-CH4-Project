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

	/** 로비 위젯 (BP에서 동일 이름으로 배치) */
	UPROPERTY(meta=(BindWidget))
	class ULobbyWidget* LobbyWidget = nullptr;

	/** 컨트롤러에서 호출하는 UI 갱신 진입점 */
	UFUNCTION(BlueprintCallable)
	void RefreshLobby();
};
