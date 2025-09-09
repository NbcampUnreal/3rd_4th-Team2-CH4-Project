#include "InGameUI/TGameResult.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UTGameResult::NativeConstruct()
{
	Super::NativeConstruct();

	if (false == ReturnToTitleButton.Get()->OnClicked.IsAlreadyBound(this, &ThisClass::OnReturnToTitleButtonClicked))
	{
		ReturnToTitleButton.Get()->OnClicked.AddDynamic(this, &ThisClass::OnReturnToTitleButtonClicked);
	}
}

void UTGameResult::OnReturnToTitleButtonClicked()
{
	UGameplayStatics::OpenLevel(GetWorld(), FName(TEXT("Title")), true);
}
