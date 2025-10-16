#include "InGameUI/TTitlePlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

void ATTitlePlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController() == false)
	{
		return;
	}

	if (IsValid(UIWidgetClass) == true)
	{
		UIWidgetInstance = CreateWidget<UUserWidget>(this, UIWidgetClass);
		if (IsValid(UIWidgetInstance) == true)
		{
			UIWidgetInstance->AddToViewport();

			FInputModeUIOnly Mode;
			//Mode.SetWidgetToFocus(UIWidgetInstance->GetCachedWidget());
			Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			SetInputMode(Mode);

			bShowMouseCursor = true;
		}
	}
}

void ATTitlePlayerController::JoinServer(const FString& InIPAddress)
{
	// InIPAddress 예: "127.0.0.1:7777"
	UE_LOG(LogTemp, Warning, TEXT("[TITLE-PLAY] ClientTravel -> %s"), *InIPAddress);
	ClientTravel(InIPAddress, ETravelType::TRAVEL_Absolute);
	// 필요 시: ClientTravel(InIPAddress, TRAVEL_Absolute, /*bSeamless=*/false);
}

void ATTitlePlayerController::Server_RequestEnterLobby_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("[TITLE-PLAY] Server_RequestEnterLobby fired. HasAuthority=%d, World=%s"),
		HasAuthority()?1:0, *GetWorld()->GetMapName());

	if (HasAuthority() && GetWorld())
	{
		UE_LOG(LogTemp, Warning, TEXT("[TITLE-PLAY] ServerTravel -> /Game/Team02/OutGameUI/Map/LobbyMap"));
		GetWorld()->ServerTravel(TEXT("/Game/Team02/OutGameUI/Map/LobbyMap?listen"));
	}
}