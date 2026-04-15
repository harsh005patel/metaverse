// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DroneRaceCheckpoint.generated.h"

class UBoxComponent;
class UMaterialInstanceDynamic;
class UStaticMeshComponent;
class UTextRenderComponent;

UCLASS(Blueprintable)
class DRONEGAME_API ADroneRaceCheckpoint : public AActor
{
	GENERATED_BODY()

public:
	ADroneRaceCheckpoint();

	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintPure, Category="Race")
	int32 GetCheckpointIndex() const { return CheckpointIndex; }

	UFUNCTION(BlueprintCallable, Category="Race")
	void SetCheckpointIndex(int32 NewCheckpointIndex);

	UFUNCTION(BlueprintPure, Category="Race")
	FVector GetGateExtent() const { return GateExtent; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UBoxComponent> TriggerBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UStaticMeshComponent> GateVisual;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UTextRenderComponent> LabelText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Race")
	int32 CheckpointIndex = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Race")
	FVector GateExtent = FVector(80.f, 500.f, 500.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Race")
	FColor GateColor = FColor::Green;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Race")
	float GateOpacity = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Race")
	bool bShowSolidGate = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Race")
	bool bDrawDebugGate = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Race", meta=(EditCondition="bDrawDebugGate", ClampMin="0.1"))
	float GateDrawThickness = 8.f;

	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> GateMaterialInstance;

private:
	UFUNCTION()
	void OnGateOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	void RefreshGateVisuals();
	void RefreshGateMaterial();
	void DrawGate() const;
};
