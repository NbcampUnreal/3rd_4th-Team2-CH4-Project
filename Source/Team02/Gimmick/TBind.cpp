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

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SetRootComponent(SphereComponent);
	SphereComponent->SetSphereRadius(1000.0f);

	// Explicitly set collision to overlap with Pawns
	SphereComponent->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComponent->SetGenerateOverlapEvents(true);
	SphereComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	SphereComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(GetRootComponent());
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void ATBind::BeginPlay()
{
	Super::BeginPlay();

	// Bind overlap events and set self-destruction timer only on the server
	if (HasAuthority())
	{
		// Set timer to destroy actor after BindDuration
		GetWorld()->GetTimerManager().SetTimer(BindTimerHandle, this, &ATBind::OnDisappear, BindDuration, false);

		SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ATBind::OnSphereOverlap);

		// Check for already overlapping actors on BeginPlay
		TArray<AActor*> OverlappingActors;
		SphereComponent->GetOverlappingActors(OverlappingActors, ACharacter::StaticClass());

		for (AActor* OverlappingActor : OverlappingActors)
		{
			ACharacter* Character = Cast<ACharacter>(OverlappingActor);
			if (Character && Character->ActorHasTag(FName("Tagger")))
			{
				ApplyBindEffect(Character);
			}
		}
	}
}

void ATBind::OnDisappear()
{
	if (HasAuthority())
	{
		// Unbind all currently affected characters
		TArray<TWeakObjectPtr<ACharacter>> BoundCharsArray = ActiveBoundCharacters.Array();
		for (TWeakObjectPtr<ACharacter> WeakChar : BoundCharsArray)
		{
			if (ACharacter* BoundChar = WeakChar.Get())
			{
				RemoveBindEffect(BoundChar);
			}
		}
		Destroy();
	}
}

void ATBind::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACharacter* Character = Cast<ACharacter>(OtherActor);
	if (Character && Character->ActorHasTag(FName("Tagger")))
	{
		ApplyBindEffect(Character);
	}
}

void ATBind::ApplyBindEffect(ACharacter* Character)
{
	if (Character && !ActiveBoundCharacters.Contains(Character))
	{
		UCharacterMovementComponent* MoveComp = Character->GetCharacterMovement();
		if (MoveComp)
		{
			MoveComp->DisableMovement();
			ActiveBoundCharacters.Add(Character);
		}
	}
}

void ATBind::RemoveBindEffect(ACharacter* Character)
{
	if (Character && ActiveBoundCharacters.Contains(Character))
	{
		UCharacterMovementComponent* MoveComp = Character->GetCharacterMovement();
		if (MoveComp)
		{
			MoveComp->SetMovementMode(EMovementMode::MOVE_Walking);
			ActiveBoundCharacters.Remove(Character);
		}
	}
}