// Copyright Epic Games, Inc. All Rights Reserved.


#include "GameModes/DroneGameGameModeBase.h"

#include "Kismet/GameplayStatics.h"
#include "Pickups/InteractableItemBase.h"

void ADroneGameGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	// Keep the playable terrain/level but strip all non-player gameplay actors.
	RemoveActorsOfClass(AInteractableItemBase::StaticClass());
}

void ADroneGameGameModeBase::RemoveActorsOfClass(TSubclassOf<AActor> ActorClass)
{
	if (!ActorClass)
	{
		return;
	}

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(this, ActorClass, FoundActors);
	for (AActor* FoundActor : FoundActors)
	{
		if (IsValid(FoundActor))
		{
			FoundActor->Destroy();
		}
	}
}
