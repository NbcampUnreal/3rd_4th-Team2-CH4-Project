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
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	BellMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BellMesh"));
	SetRootComponent(BellMesh);

	DetectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DetectionSphere"));
	DetectionSphere->SetupAttachment(BellMesh);
	DetectionSphere->SetSphereRadius(DetectionRadius);
	DetectionSphere->SetCollisionProfileName(TEXT("OverlapAll")); // Or a custom profile
	DetectionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	DetectionSphere->SetGenerateOverlapEvents(true);

	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	AudioComponent->SetupAttachment(BellMesh);
	AudioComponent->bAutoActivate = false; // Don't play on spawn

	CurrentSoundPlayedTime = 0.0f;
	bIsSoundPlaying = false;
}

void ATBell::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATBell, CurrentSoundPlayedTime);
	DOREPLIFETIME(ATBell, bIsSoundPlaying);
}

// Called when the game starts or when spawned
void ATBell::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		DetectionSphere->OnComponentBeginOverlap.AddDynamic(this, &ATBell::OnOverlapBegin);
		DetectionSphere->OnComponentEndOverlap.AddDynamic(this, &ATBell::OnOverlapEnd);

		// Initial check for overlapping actors
		TArray<AActor*> OverlappingActors;
		DetectionSphere->GetOverlappingActors(OverlappingActors, ATCharacter::StaticClass());
		for (AActor* Actor : OverlappingActors)
		{
			OnOverlapBegin(DetectionSphere, Actor, Cast<UPrimitiveComponent>(Actor->GetRootComponent()), 0, false, FHitResult());
		}
	}
}

// Called every frame
void ATBell::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsSoundPlaying)
	{
		CurrentSoundPlayedTime += DeltaTime;
		if (CurrentSoundPlayedTime >= MaxSoundDuration)
		{
			OnSoundDurationTimerEnd();
		}
	}
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
				OverlappingThieves.Add(OtherCharacter);
				UpdateBellSoundState();
			}
		}
	}
}

void ATBell::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (HasAuthority() && OtherActor)
	{
		ATCharacter* OtherCharacter = Cast<ATCharacter>(OtherActor);
		if (OtherCharacter)
		{
			OverlappingThieves.Remove(OtherCharacter);
			UpdateBellSoundState();
		}
	}
}

void ATBell::UpdateBellSoundState()
{
	if (OverlappingThieves.Num() > 0 && CurrentSoundPlayedTime < MaxSoundDuration)
	{
		// If there are thieves nearby and sound is not playing, start it
		if (BellSound && (!AudioComponent->IsPlaying()))
		{
			AudioComponent->SetSound(BellSound);
			AudioComponent->Play();
			bIsSoundPlaying = true;
		}
	}
	else
	{
		// No thieves nearby or max duration reached, stop the sound
		if (AudioComponent->IsPlaying())
		{
			AudioComponent->Stop();
			bIsSoundPlaying = false;
		}
	}
}

void ATBell::OnSoundDurationTimerEnd()
{
	if (AudioComponent->IsPlaying())
	{
		AudioComponent->Stop();
	}
	bIsSoundPlaying = false;
	// Optionally destroy the bell actor after its sound duration is over
	// Destroy();
}

