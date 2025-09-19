// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/TBell.h"
#include "Components/SphereComponent.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Character/TCharacter.h"
#include "OutGameUI/TTeamTypes.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ATBell::ATBell()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	BellMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BellMesh"));
	SetRootComponent(BellMesh);

	DetectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DetectionSphere"));
	DetectionSphere->SetupAttachment(BellMesh);
	DetectionSphere->SetSphereRadius(DetectionRadius);
	DetectionSphere->SetCollisionProfileName(TEXT("OverlapAll"));
	DetectionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	DetectionSphere->SetGenerateOverlapEvents(true);

	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	AudioComponent->SetupAttachment(BellMesh);
	AudioComponent->bAutoActivate = false;
}

void ATBell::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}

// Called when the game starts or when spawned
void ATBell::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority()) // Only run on the server
	{
		// Set a timer to destroy the actor after MaxSoundDuration
		GetWorldTimerManager().SetTimer(SoundDurationTimerHandle, this, &ATBell::OnSoundDurationTimerEnd, MaxSoundDuration, false);

		// Bind overlap events
		DetectionSphere->OnComponentBeginOverlap.AddDynamic(this, &ATBell::OnOverlapBegin);

		// Initial check for already overlapping actors
		TArray<AActor*> OverlappingActors;
		DetectionSphere->GetOverlappingActors(OverlappingActors, ATCharacter::StaticClass());
		for (AActor* Actor : OverlappingActors)
		{
			ATCharacter* Character = Cast<ATCharacter>(Actor);
			if (Character && Character->ActorHasTag(FName("Hider")))
			{
				UpdateBellSoundState(); // Trigger sound state update
				break; // Only need to find one Hider to play sound
			}
		}
	}
}

// Called every frame
void ATBell::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// No longer managing destruction via Tick, handled by timer in BeginPlay
}

void ATBell::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (HasAuthority() && OtherActor)
	{
		ATCharacter* OtherCharacter = Cast<ATCharacter>(OtherActor);
		if (OtherCharacter)
		{
			// Check if the overlapping character has the "Hider" tag.
			if (OtherCharacter->ActorHasTag(FName("Hider")))
			{
				UpdateBellSoundState(); // Trigger sound state update
			}
		}
	}
}


void ATBell::UpdateBellSoundState()
{
	// If BellSound is assigned and AudioComponent is not already playing
	if (BellSound && (!AudioComponent->IsPlaying()))
	{
		ClientPlayBellSound(); // Call Client RPC to play sound
	}
}

void ATBell::OnSoundDurationTimerEnd()
{
	if (AudioComponent->IsPlaying())
	{
		AudioComponent->Stop();
	}
	Destroy();
}

void ATBell::ClientPlayBellSound_Implementation()
{
	if (BellSound && !AudioComponent->IsPlaying())
	{
		AudioComponent->SetSound(BellSound);
		AudioComponent->Play();
	}
}
