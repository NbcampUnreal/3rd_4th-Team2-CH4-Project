#include "NPC/TNpcCharacter.h"


ATNpcCharacter::ATNpcCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

}

void ATNpcCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATNpcCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATNpcCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

