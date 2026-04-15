// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DroneRaceFinishLine.generated.h"

class UBoxComponent;
class UTextRenderComponent;

UCLASS(Blueprintable)
class DRONEGAME_API ADroneRaceFinishLine : public AActor
{
	GENERATED_BODY()

public:
	ADroneRaceFinishLine();

	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintPure, Category="Race")
	FVector GetGateExtent() const { return GateExtent; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UBoxComponent> TriggerBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UTextRenderComponent> LabelText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Race")
	FVector GateExtent = FVector(80.f, 600.f, 400.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Race")
	FColor GateColor = FColor::Yellow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Race")
	bool bDrawGate = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Race", meta=(EditCondition="bDrawGate", ClampMin="0.1"))
	float GateDrawThickness = 10.f;

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
	void DrawGate() const;
};
