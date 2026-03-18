// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"
#include "Pawns/DronePawnBase.h"
#include "DroneMovementComponent.generated.h"

/**
 * Drone Movement Component class
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DRONEGAME_API UDroneMovementComponent : public UPawnComponent
{
	GENERATED_BODY()

public:
	UDroneMovementComponent(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable)
	virtual void AddDroneMovementInput(const FVector& Value);
	UFUNCTION(BlueprintCallable)
	virtual void AddDroneLookInput(const FVector& Value);
	UFUNCTION(BlueprintCallable)
	virtual void AddDroneThrottle(float Value);
	
	void SetComponentToMove(UPrimitiveComponent* InComp);
	void SetThrusterComponent(UPhysicsThrusterComponent* InComp);
	
	UPrimitiveComponent* GetMovableComponent() const {return ComponentToMove;}
	
	UFUNCTION(BlueprintPure)
	float GetThrottlePercent() const;
	
	UFUNCTION(BlueprintPure)
	float GetVelocityLength() const;

	UFUNCTION(BlueprintPure)
	FVector GetVelocity() const;

	UFUNCTION(BlueprintPure)
	bool IsVelocityDown() const;
	
	UFUNCTION(BlueprintCallable)
	void DisableAllMovement();
	UFUNCTION(BlueprintCallable)
	void EnableMovement();
protected:
	virtual void BeginPlay() override;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	float HorizontalSpeed = 1200.f;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	float VerticalSpeed = 600.f;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	float YawSpeed = 90.f;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	float MaxTiltAngle = 20.f;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	float AutoLevelSpeed = 5.f;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	float HorizontalAccelerationSpeed = 4.f;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	float VerticalAccelerationSpeed = 3.f;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	float YawAccelerationSpeed = 5.f;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	float VelocityDamping = 0.98f;

	UPROPERTY(BlueprintReadOnly)
	float CurrentThrottle;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	float ThrottleSpeed = 120.f;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	float MinThrottle = -250.f;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	float MaxThrottle = 250.f;

private:
	void UpdateInputTimestamp(float& Timestamp) const;
	void ClearExpiredRemoteInputs(const APawn* OwningPawn);

	UPROPERTY()
	UPrimitiveComponent* ComponentToMove;

	UPROPERTY()
	UPhysicsThrusterComponent* ThrusterComponent;

	UPROPERTY(BlueprintReadOnly,meta=(AllowPrivateAccess))
	float ThrottleInput;
	UPROPERTY(BlueprintReadOnly,meta=(AllowPrivateAccess))
	FVector MoveInput;
	UPROPERTY(BlueprintReadOnly,meta=(AllowPrivateAccess))
	FVector LookInput;

	UPROPERTY(BlueprintReadOnly,meta=(AllowPrivateAccess))
	FVector CachedVelocity = FVector::ZeroVector;
	UPROPERTY(BlueprintReadOnly,meta=(AllowPrivateAccess))
	FVector CurrentVelocity = FVector::ZeroVector;
	UPROPERTY(BlueprintReadOnly,meta=(AllowPrivateAccess))
	FVector PreviousLocation = FVector::ZeroVector;
	UPROPERTY(BlueprintReadOnly,meta=(AllowPrivateAccess))
	float CurrentYaw = 0.f;
	UPROPERTY(BlueprintReadOnly,meta=(AllowPrivateAccess))
	float CurrentYawVelocity = 0.f;

	UPROPERTY(BlueprintReadOnly,meta=(AllowPrivateAccess))
	bool bEnableMovement = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess, ClampMin="0.01"))
	float RemoteInputTimeout = 0.15f;

	float LastMoveInputTime = -1.f;
	float LastLookInputTime = -1.f;
	float LastThrottleInputTime = -1.f;
};
