// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "DronePawnBase.generated.h"

class UDroneMovementComponent;
class UPhysicsThrusterComponent;
class UBoxComponent;

UCLASS(Abstract)
class DRONEGAME_API ADronePawnBase : public APawn
{
	GENERATED_BODY()

public:
	ADronePawnBase();

	virtual void Tick(float DeltaTime) override;

	FORCEINLINE UDroneMovementComponent* GetDroneMovementComponent() const {return DroneMovementComponent;}

	void HandleMoveInput(const FVector& Value);
	void HandleLookInput(const FVector& Value);
	void HandleThrottleInput(float Value);
protected:
	virtual void BeginPlay() override;

	UFUNCTION(Server, Unreliable)
	void ServerHandleMoveInput(const FVector& Value);

	UFUNCTION(Server, Unreliable)
	void ServerHandleLookInput(const FVector& Value);

	UFUNCTION(Server, Unreliable)
	void ServerHandleThrottleInput(float Value);

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	TObjectPtr<UBoxComponent> BoxComponent;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> SkeletalMesh;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	TObjectPtr<UPhysicsThrusterComponent> PhysicsThruster;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	TObjectPtr<UDroneMovementComponent> DroneMovementComponent;
};
