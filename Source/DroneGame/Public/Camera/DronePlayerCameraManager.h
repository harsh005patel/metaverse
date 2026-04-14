// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "DronePlayerCameraManager.generated.h"

UENUM(BlueprintType)
enum class EPlayerViewMode : uint8
{
	FirstPerson,
	ThirdPerson
};

USTRUCT(BlueprintType)
struct FViewModeSpringArmParams
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	float TargetArmLength = 0.f;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	FVector TargetLocation = FVector::ZeroVector;
};

/**
 * Drone player camera manager class
 */
UCLASS()
class DRONEGAME_API ADronePlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void ChangeViewMode(EPlayerViewMode PlayerViewMode);
	
	UFUNCTION(BlueprintPure)
	EPlayerViewMode GetCurrentViewMode() const {return CurrentViewMode;}
protected:
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	TMap<EPlayerViewMode,FViewModeSpringArmParams> ViewMods;
private:
	EPlayerViewMode CurrentViewMode = EPlayerViewMode::ThirdPerson;
};
