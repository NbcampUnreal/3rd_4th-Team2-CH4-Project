// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TBind.generated.h"

class ACharacter;
class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class TEAM02_API ATBind : public AActor
{
	GENERATED_BODY()
	
public:	
	ATBind();

protected:
	virtual void BeginPlay() override;

	void RemoveBind();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USphereComponent> SphereComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(EditAnywhere, Category = "Gimmick")
	float BindDuration = 2.0f;

	FTimerHandle BindTimerHandle;

	UPROPERTY()
	TArray<TWeakObjectPtr<ACharacter>> AffectedCharacters;
};
