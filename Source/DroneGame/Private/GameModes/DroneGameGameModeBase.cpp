// Copyright Epic Games, Inc. All Rights Reserved.


#include "GameModes/DroneGameGameModeBase.h"

#include "Engine/Engine.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Pickups/InteractableItemBase.h"
#include "Player/DroneGamePlayerController.h"
#include "Race/DroneRaceCheckpoint.h"
#include "Race/DroneRaceFinishLine.h"

void ADroneGameGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	// Keep the playable terrain/level but strip all non-player gameplay actors.
	RemoveActorsOfClass(AInteractableItemBase::StaticClass());

	InitializeRaceCourse();
}

void ADroneGameGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	StartRaceForController(NewPlayer);
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

void ADroneGameGameModeBase::InitializeRaceCourse()
{
	CollectRaceActors();

	if (bSpawnDemoRaceCourseIfMissing && Checkpoints.IsEmpty() && !FinishLine)
	{
		SpawnDemoRaceCourse();
		CollectRaceActors();
	}

	ResetRaceProgress();

	UE_LOG(LogTemp, Warning, TEXT("[Race] Ready with %d checkpoint(s)%s."),
		Checkpoints.Num(),
		FinishLine ? TEXT(" and a finish line") : TEXT(" but no finish line"));
}

void ADroneGameGameModeBase::CollectRaceActors()
{
	Checkpoints.Reset();
	FinishLine = nullptr;

	TArray<AActor*> FoundCheckpointActors;
	UGameplayStatics::GetAllActorsOfClass(this, ADroneRaceCheckpoint::StaticClass(), FoundCheckpointActors);
	for (AActor* FoundActor : FoundCheckpointActors)
	{
		if (ADroneRaceCheckpoint* Checkpoint = Cast<ADroneRaceCheckpoint>(FoundActor))
		{
			Checkpoints.Add(Checkpoint);
		}
	}

	Checkpoints.Sort([](const ADroneRaceCheckpoint& Left, const ADroneRaceCheckpoint& Right)
	{
		return Left.GetCheckpointIndex() < Right.GetCheckpointIndex();
	});

	TArray<AActor*> FoundFinishActors;
	UGameplayStatics::GetAllActorsOfClass(this, ADroneRaceFinishLine::StaticClass(), FoundFinishActors);
	if (!FoundFinishActors.IsEmpty())
	{
		FinishLine = Cast<ADroneRaceFinishLine>(FoundFinishActors[0]);
	}
}

void ADroneGameGameModeBase::SpawnDemoRaceCourse()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	TArray<AActor*> PlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);

	const FTransform BaseTransform = PlayerStarts.IsEmpty()
		? FTransform(FRotator::ZeroRotator, FVector::ZeroVector)
		: PlayerStarts[0]->GetActorTransform();

	const FVector Forward = BaseTransform.GetRotation().GetForwardVector();
	const FVector SpawnBaseLocation = BaseTransform.GetLocation() + FVector(0.f, 0.f, DemoGateHeight);
	const FRotator GateRotation = BaseTransform.GetRotation().Rotator();

	for (int32 Index = 0; Index < DemoCheckpointCount; ++Index)
	{
		const FVector Location = SpawnBaseLocation + Forward * (DemoFirstCheckpointDistance + DemoCheckpointSpacing * Index);
		ADroneRaceCheckpoint* Checkpoint = World->SpawnActor<ADroneRaceCheckpoint>(
			ADroneRaceCheckpoint::StaticClass(),
			Location,
			GateRotation);

		if (Checkpoint)
		{
			Checkpoint->SetCheckpointIndex(Index + 1);
		}
	}

	const FVector FinishLocation = SpawnBaseLocation + Forward * (DemoFirstCheckpointDistance + DemoCheckpointSpacing * DemoCheckpointCount);
	FinishLine = World->SpawnActor<ADroneRaceFinishLine>(
		ADroneRaceFinishLine::StaticClass(),
		FinishLocation,
		GateRotation);
}

void ADroneGameGameModeBase::ResetRaceProgress()
{
	NextCheckpointByController.Reset();
	FinishedControllers.Reset();

	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		if (AController* Controller = Iterator->Get())
		{
			StartRaceForController(Controller);
		}
	}
}

void ADroneGameGameModeBase::StartRaceForController(AController* Controller)
{
	if (!Controller)
	{
		return;
	}

	NextCheckpointByController.FindOrAdd(Controller) = 0;
	FinishedControllers.Remove(Controller);

	SendRaceMessage(
		Controller,
		Checkpoints.IsEmpty()
			? TEXT("Race ready. Fly through the finish line.")
			: FString::Printf(TEXT("Race ready. Next checkpoint: 1/%d"), Checkpoints.Num()),
		FColor::Green,
		6.f);
}

void ADroneGameGameModeBase::HandleCheckpointReached(ADroneRaceCheckpoint* Checkpoint, APawn* Pawn)
{
	if (!Checkpoint || !Pawn)
	{
		return;
	}

	AController* Controller = Pawn->GetController();
	if (!Controller || FinishedControllers.Contains(Controller))
	{
		return;
	}

	int32& NextCheckpoint = NextCheckpointByController.FindOrAdd(Controller);
	const int32 CheckpointArrayIndex = Checkpoints.IndexOfByKey(Checkpoint);
	if (CheckpointArrayIndex == INDEX_NONE)
	{
		return;
	}

	if (CheckpointArrayIndex < NextCheckpoint)
	{
		return;
	}

	if (CheckpointArrayIndex > NextCheckpoint)
	{
		SendRaceMessage(
			Controller,
			FString::Printf(TEXT("Wrong checkpoint. Go to checkpoint %d first."), NextCheckpoint + 1),
			FColor::Orange);
		return;
	}

	++NextCheckpoint;

	SendRaceMessage(
		Controller,
		NextCheckpoint >= Checkpoints.Num()
			? TEXT("All checkpoints cleared. Fly to the finish line.")
			: FString::Printf(TEXT("Checkpoint %d/%d"), NextCheckpoint, Checkpoints.Num()),
		FColor::Cyan);
}

void ADroneGameGameModeBase::HandleFinishLineReached(ADroneRaceFinishLine* FinishLineActor, APawn* Pawn)
{
	if (!FinishLineActor || FinishLineActor != FinishLine || !Pawn)
	{
		return;
	}

	AController* Controller = Pawn->GetController();
	if (!Controller || FinishedControllers.Contains(Controller))
	{
		return;
	}

	if (!HasCompletedAllCheckpoints(Controller))
	{
		const int32 NextCheckpoint = GetNextCheckpointIndexForController(Controller);
		SendRaceMessage(
			Controller,
			FString::Printf(TEXT("Finish locked. Complete checkpoint %d/%d first."), NextCheckpoint + 1, Checkpoints.Num()),
			FColor::Orange);
		return;
	}

	FinishedControllers.Add(Controller);
	SendRaceMessage(Controller, TEXT("Finished! Race complete."), FColor::Green, 8.f);
}

int32 ADroneGameGameModeBase::GetNextCheckpointIndexForController(AController* Controller) const
{
	if (!Controller)
	{
		return 0;
	}

	if (const int32* NextCheckpoint = NextCheckpointByController.Find(Controller))
	{
		return *NextCheckpoint;
	}

	return 0;
}

void ADroneGameGameModeBase::SendRaceMessage(AController* Controller, const FString& Message, const FColor& Color, float Duration) const
{
	if (ADroneGamePlayerController* DroneController = Cast<ADroneGamePlayerController>(Controller))
	{
		DroneController->ClientShowGameMessage(Message, Color, Duration);
	}

	UE_LOG(LogTemp, Warning, TEXT("[Race] %s"), *Message);
}

bool ADroneGameGameModeBase::HasCompletedAllCheckpoints(AController* Controller) const
{
	return GetNextCheckpointIndexForController(Controller) >= Checkpoints.Num();
}
