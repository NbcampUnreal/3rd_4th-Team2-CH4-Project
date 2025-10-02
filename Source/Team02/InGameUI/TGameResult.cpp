#include "InGameUI/TGameResult.h"
#include "Components/Button.h"
#include "OutGameUI/TUPlayerController.h" 
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
	//UGameplayStatics::OpenLevel(GetWorld(), FName(TEXT("Title")), true);
	if (APlayerController* PC = GetOwningPlayer())
	{
		if (ATUPlayerController* TPC = Cast<ATUPlayerController>(PC))
		{
			// 서버에 요청 → 서버가 ServerTravel로 모두 이동
			TPC->Server_ReturnToTitle();
		}
	}
}
