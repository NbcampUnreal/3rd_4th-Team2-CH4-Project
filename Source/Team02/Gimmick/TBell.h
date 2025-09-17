// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "Sound/SoundBase.h"
#include "Components/AudioComponent.h"
#include "Components/StaticMeshComponent.h" // Added for BellMesh
#include "TBell.generated.h"

class ATCharacter; // Still needed for OverlappingThieves

UCLASS()
class TEAM02_API ATBell : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> BellMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> DetectionSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UAudioComponent> AudioComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	TObjectPtr<USoundBase> BellSound;

	UPROPERTY(EditDefaultsOnly, Category = "Detection")
	float DetectionRadius = 1000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	float MaxSoundDuration = 5.0f;

	UPROPERTY(Replicated)
	float CurrentSoundPlayedTime;

	UPROPERTY(Replicated)
	bool bIsSoundPlaying;

	UPROPERTY()
	TSet<TWeakObjectPtr<ATCharacter>> OverlappingThieves;

	FTimerHandle SoundDurationTimerHandle;

public:
	// Sets default values for this actor's properties
	ATBell();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void UpdateBellSoundState();
	void OnSoundDurationTimerEnd();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};