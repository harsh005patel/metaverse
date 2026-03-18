#include "DroneGameInstance.h"

#include "Engine/Engine.h"
#include "Engine/NetDriver.h"

namespace DroneGameNetworkDebug
{
	static FString GetNetworkFailureName(ENetworkFailure::Type FailureType)
	{
		switch (FailureType)
		{
		case ENetworkFailure::NetDriverAlreadyExists:
			return TEXT("NetDriverAlreadyExists");
		case ENetworkFailure::NetDriverCreateFailure:
			return TEXT("NetDriverCreateFailure");
		case ENetworkFailure::NetDriverListenFailure:
			return TEXT("NetDriverListenFailure");
		case ENetworkFailure::ConnectionLost:
			return TEXT("ConnectionLost");
		case ENetworkFailure::ConnectionTimeout:
			return TEXT("ConnectionTimeout");
		case ENetworkFailure::FailureReceived:
			return TEXT("FailureReceived");
		case ENetworkFailure::OutdatedClient:
			return TEXT("OutdatedClient");
		case ENetworkFailure::OutdatedServer:
			return TEXT("OutdatedServer");
		case ENetworkFailure::PendingConnectionFailure:
			return TEXT("PendingConnectionFailure");
		case ENetworkFailure::NetGuidMismatch:
			return TEXT("NetGuidMismatch");
		case ENetworkFailure::NetChecksumMismatch:
			return TEXT("NetChecksumMismatch");
		default:
			return TEXT("UnknownNetworkFailure");
		}
	}

	static FString GetTravelFailureName(ETravelFailure::Type FailureType)
	{
		switch (FailureType)
		{
		case ETravelFailure::NoLevel:
			return TEXT("NoLevel");
		case ETravelFailure::LoadMapFailure:
			return TEXT("LoadMapFailure");
		case ETravelFailure::InvalidURL:
			return TEXT("InvalidURL");
		case ETravelFailure::PackageMissing:
			return TEXT("PackageMissing");
		case ETravelFailure::PackageVersion:
			return TEXT("PackageVersion");
		case ETravelFailure::NoDownload:
			return TEXT("NoDownload");
		case ETravelFailure::TravelFailure:
			return TEXT("TravelFailure");
		case ETravelFailure::CheatCommands:
			return TEXT("CheatCommands");
		case ETravelFailure::PendingNetGameCreateFailure:
			return TEXT("PendingNetGameCreateFailure");
		case ETravelFailure::CloudSaveFailure:
			return TEXT("CloudSaveFailure");
		case ETravelFailure::ServerTravelFailure:
			return TEXT("ServerTravelFailure");
		case ETravelFailure::ClientTravelFailure:
			return TEXT("ClientTravelFailure");
		default:
			return TEXT("UnknownTravelFailure");
		}
	}

	static FString GetWorldName(const UWorld* World)
	{
		return World ? World->GetMapName() : FString(TEXT("NoWorld"));
	}
}

void UDroneGameInstance::Init()
{
	Super::Init();

	if (GEngine)
	{
		NetworkFailureHandle = GEngine->OnNetworkFailure().AddUObject(this, &ThisClass::HandleNetworkFailure);
		TravelFailureHandle = GEngine->OnTravelFailure().AddUObject(this, &ThisClass::HandleTravelFailure);
	}

	ShowNetworkMessage(TEXT("Network diagnostics enabled. Use HostListen / JoinServer / NetworkStatus in the console."));
}

void UDroneGameInstance::Shutdown()
{
	if (GEngine)
	{
		if (NetworkFailureHandle.IsValid())
		{
			GEngine->OnNetworkFailure().Remove(NetworkFailureHandle);
		}

		if (TravelFailureHandle.IsValid())
		{
			GEngine->OnTravelFailure().Remove(TravelFailureHandle);
		}
	}

	Super::Shutdown();
}

void UDroneGameInstance::ShowNetworkMessage(const FString& Message, const FColor& Color, float Duration) const
{
	UE_LOG(LogTemp, Warning, TEXT("[NetworkInfo] %s"), *Message);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, Duration, Color, Message);
	}
}

void UDroneGameInstance::HandleNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType,
	const FString& ErrorString)
{
	const FString FailureName = DroneGameNetworkDebug::GetNetworkFailureName(FailureType);
	const FString NetDriverName = NetDriver ? NetDriver->GetName() : FString(TEXT("NoNetDriver"));

	ShowNetworkMessage(
		FString::Printf(
			TEXT("Network failure: %s | Driver: %s | World: %s | Error: %s"),
			*FailureName,
			*NetDriverName,
			*DroneGameNetworkDebug::GetWorldName(World),
			*ErrorString),
		FColor::Red,
		10.f);
}

void UDroneGameInstance::HandleTravelFailure(UWorld* World, ETravelFailure::Type FailureType,
	const FString& ErrorString)
{
	const FString FailureName = DroneGameNetworkDebug::GetTravelFailureName(FailureType);

	ShowNetworkMessage(
		FString::Printf(
			TEXT("Travel failure: %s | World: %s | Error: %s"),
			*FailureName,
			*DroneGameNetworkDebug::GetWorldName(World),
			*ErrorString),
		FColor::Red,
		10.f);
}
