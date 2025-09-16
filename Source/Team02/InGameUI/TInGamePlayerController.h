#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TInGamePlayerController.generated.h"

class UTInGameHUD;

/**
 * 인게임 동안 HUD를 생성하고, GameState 델리게이트/타이머에 바인딩하는 컨트롤러.
 * - 입력 매핑은 기존 ATPlayerController 쪽에 구성되어 있음
 * - 여기서는 HUD 생성/표시/데이터 바인딩만 수행.
 */
UCLASS()
class TEAM02_API ATInGamePlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    virtual void BeginPlay() override;

protected:
    // BP_InGameHUD를 부모로 두는 C++ HUD 위젯 클래스
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UTInGameHUD> InGameHUDClass;

    //실제 생성된 HUD 인스턴스
    UPROPERTY()
    TObjectPtr<UTInGameHUD> InGameHUDInstance = nullptr;
};
