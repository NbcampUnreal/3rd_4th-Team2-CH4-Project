// Fill out your copyright notice in the Description page of Project Settings.

#include "Gimmick/TSpeedup.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"
//#include "Character/HCharacter.h" // Include HCharacter.h

// Sets default values
ATSpeedup::ATSpeedup()
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
}

// Called when the game starts or when spawned
void ATSpeedup::BeginPlay()
{
	Super::BeginPlay();

	// Bind the overlap event only on the server
	if (HasAuthority())
	{
		SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ATSpeedup::OnSphereOverlap);
	}
}

void ATSpeedup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	/*
	AHCharacter* HiderCharacter = Cast<AHCharacter>(OtherActor);
	if (HiderCharacter && HiderCharacter->ActorHasTag(FName("Hider")))
	{
		// Apply speed buff through HCharacter's function
		HiderCharacter->ApplySpeedBuff();
		AffectedHider = HiderCharacter; // Store the affected character

		// Set a timer to remove the speed buff
		GetWorld()->GetTimerManager().SetTimer(SpeedBuffTimerHandle, this, &ATSpeedup::RemoveSpeedBuff, BuffDuration, false);

		// Disable further overlaps to prevent re-triggering
		SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	*/
}

void ATSpeedup::RemoveSpeedBuff()
{
	/*
	if (AffectedHider.IsValid())
	{
		AffectedHider->RemoveSpeedBuff();
	}
	*/
	Destroy();
}