// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/TPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

ATPlayerController::ATPlayerController() : MoveAction(nullptr), LookAction(nullptr),SprintAction(nullptr), DefaultMappingContext(nullptr)
{
	// Set this player controller to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ATPlayerController::BeginPlay()
{
	Super::BeginPlay();
	// Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}
}