#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TGameResult.generated.h"

class UTextBlock;
class UButton;
/**
 * 
 */
UCLASS()
class TEAM02_API UTGameResult : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

private:
	UFUNCTION()
	void OnReturnToTitleButtonClicked();
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (BindWidget))
	TObjectPtr<UTextBlock> ResultText;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (BindWidget))
	TObjectPtr<UButton> ReturnToTitleButton;
};
