// Fill out your copyright notice in the Description page of Project Settings.

#include "Gimmick/TBind.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"

// Sets default values
ATBind::ATBind()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	InitialLifeSpan = 4.0f; // Failsafe

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SetRootComponent(SphereComponent);
	SphereComponent->SetSphereRadius(1000.0f); // Default 10m radius, editable in Blueprint
	SphereComponent->SetCollisionProfileName(FName("OverlapAllDynamic"));

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(GetRootComponent());
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void ATBind::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		TArray<AActor*> OverlappedActors;
		SphereComponent->GetOverlappingActors(OverlappedActors, ACharacter::StaticClass());

		for (AActor* OverlappedActor : OverlappedActors)
		{
			ACharacter* Character = Cast<ACharacter>(OverlappedActor);
			if (Character && Character->ActorHasTag(FName("Tagger")))
			{
				UCharacterMovementComponent* MoveComp = Character->GetCharacterMovement();
				if (MoveComp)
				{
					MoveComp->DisableMovement();
					AffectedCharacters.Add(Character);
				}
			}
		}

		if (AffectedCharacters.Num() > 0)
		{
			GetWorld()->GetTimerManager().SetTimer(BindTimerHandle, this, &ATBind::RemoveBind, BindDuration, false);
		}
		else
		{
			Destroy();
		}
	}
}

void ATBind::RemoveBind()
{
	for (TWeakObjectPtr<ACharacter> CharacterPtr : AffectedCharacters)
	{
		if (CharacterPtr.IsValid())
		{
			UCharacterMovementComponent* MoveComp = CharacterPtr->GetCharacterMovement();
			if (MoveComp)
			{
				MoveComp->SetMovementMode(EMovementMode::MOVE_Walking);
			}
		}
	}

	Destroy();
}
