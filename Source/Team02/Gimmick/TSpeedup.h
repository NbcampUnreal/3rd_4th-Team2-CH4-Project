// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TSpeedup.generated.h"

class ACharacter;
class USphereComponent;
class UStaticMeshComponent;


UCLASS()
class TEAM02_API ATSpeedup : public AActor
{
	GENERATED_BODY()

public:
	ATSpeedup();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USphereComponent> SphereComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(EditAnywhere, Category = "Gimmick")
	float SpeedMultiplier = 1.5f;

	UPROPERTY(EditAnywhere, Category = "Gimmick")
	float BuffDuration = 3.0f;
};