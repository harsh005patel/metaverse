// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DroneGameGameModeBase.generated.h"

class ADroneRaceCheckpoint;
class ADroneRaceFinishLine;

/**
 * DroneGame Base GameMode class 
 */
UCLASS()
class DRONEGAME_API ADroneGameGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;

	void HandleCheckpointReached(ADroneRaceCheckpoint* Checkpoint, APawn* Pawn);
	void HandleFinishLineReached(ADroneRaceFinishLine* FinishLineActor, APawn* Pawn);

	UFUNCTION(BlueprintPure, Category="Race")
	int32 GetCheckpointCount() const { return Checkpoints.Num(); }

	UFUNCTION(BlueprintPure, Category="Race")
	int32 GetNextCheckpointIndexForController(AController* Controller) const;

private:
	void RemoveActorsOfClass(TSubclassOf<AActor> ActorClass);
	void InitializeRaceCourse();
	void CollectRaceActors();
	void SpawnDemoRaceCourse();
	void ResetRaceProgress();
	void StartRaceForController(AController* Controller);
	void SendRaceMessage(AController* Controller, const FString& Message, const FColor& Color, float Duration = 4.f) const;
	bool HasCompletedAllCheckpoints(AController* Controller) const;

	UPROPERTY(EditDefaultsOnly, Category="Race")
	bool bSpawnDemoRaceCourseIfMissing = true;

	UPROPERTY(EditDefaultsOnly, Category="Race", meta=(ClampMin="0"))
	int32 DemoCheckpointCount = 3;

	UPROPERTY(EditDefaultsOnly, Category="Race")
	float DemoFirstCheckpointDistance = 1200.f;

	UPROPERTY(EditDefaultsOnly, Category="Race")
	float DemoCheckpointSpacing = 1200.f;

	UPROPERTY(EditDefaultsOnly, Category="Race")
	float DemoGateHeight = 450.f;

	UPROPERTY()
	TArray<TObjectPtr<ADroneRaceCheckpoint>> Checkpoints;

	UPROPERTY()
	TObjectPtr<ADroneRaceFinishLine> FinishLine;

	TMap<TWeakObjectPtr<AController>, int32> NextCheckpointByController;
	TSet<TWeakObjectPtr<AController>> FinishedControllers;
};
