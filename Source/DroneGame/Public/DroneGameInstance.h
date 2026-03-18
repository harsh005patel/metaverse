#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineBaseTypes.h"
#include "Engine/GameInstance.h"
#include "DroneGameInstance.generated.h"

class UNetDriver;

UCLASS()
class DRONEGAME_API UDroneGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;
	virtual void Shutdown() override;

	void ShowNetworkMessage(const FString& Message, const FColor& Color = FColor::Cyan, float Duration = 6.f) const;

private:
	void HandleNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString);
	void HandleTravelFailure(UWorld* World, ETravelFailure::Type FailureType, const FString& ErrorString);

	FDelegateHandle NetworkFailureHandle;
	FDelegateHandle TravelFailureHandle;
};
