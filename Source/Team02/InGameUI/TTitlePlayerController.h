#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TTitlePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class TEAM02_API ATTitlePlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;

	void JoinServer(const FString& InIPAddress);

	UFUNCTION(Server, Reliable)
	void Server_RequestEnterLobby();  
	
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = ASUIPlayerController, Meta = (AllowPrivateAccess))
	TSubclassOf<UUserWidget> UIWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = ASUIPlayerController, Meta = (AllowPrivateAccess))
	TObjectPtr<UUserWidget> UIWidgetInstance;
};
