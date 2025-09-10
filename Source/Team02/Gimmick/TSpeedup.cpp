// Fill out your copyright notice in the Description page of Project Settings.

#include "Gimmick/TSpeedup.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"

// Sets default values
ATSpeedup::ATSpeedup()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	InitialLifeSpan = 5.0f; // Failsafe

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SetRootComponent(SphereComponent);
	SphereComponent->SetSphereRadius(1000.0f); // Default 10m radius
	SphereComponent->SetCollisionProfileName(FName("OverlapAllDynamic"));
}

// Called when the game starts or when spawned
void ATSpeedup::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		TArray<AActor*> OverlappedActors;
		SphereComponent->GetOverlappingActors(OverlappedActors, ACharacter::StaticClass());

		for (AActor* OverlappedActor : OverlappedActors)
		{
			ACharacter* Character = Cast<ACharacter>(OverlappedActor);
			if (Character && Character->ActorHasTag(FName("Hider")))
			{
				UCharacterMovementComponent* MoveComp = Character->GetCharacterMovement();
				if (MoveComp)
				{
					AffectedCharacters.Add(Character, MoveComp->MaxWalkSpeed);
					MoveComp->MaxWalkSpeed *= SpeedMultiplier;
				}
			}
		}

		if (AffectedCharacters.Num() > 0)
		{
			GetWorld()->GetTimerManager().SetTimer(SpeedBuffTimerHandle, this, &ATSpeedup::RemoveSpeedBuff, BuffDuration, false);
		}
		else
		{
			Destroy();
		}
	}
}

void ATSpeedup::RemoveSpeedBuff()
{
	for (auto const& Pair : AffectedCharacters)
	{
		TWeakObjectPtr<ACharacter> CharacterPtr = Pair.Key;
		if (CharacterPtr.IsValid())
		{
			UCharacterMovementComponent* MoveComp = CharacterPtr->GetCharacterMovement();
			if (MoveComp)
			{
				MoveComp->MaxWalkSpeed = Pair.Value; // Restore original speed
			}
		}
	}

	Destroy();
}