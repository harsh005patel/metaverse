// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/DroneGamePlayerController.h"
#include "DroneGameInstance.h"
#include "Blueprint/UserWidget.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY_STATIC(LogDroneGamePlayerController, Log, All);

namespace DroneGamePlayerControllerDebug
{
	static FString GetNetModeString(const UWorld* World)
	{
		if (!World)
		{
			return TEXT("NoWorld");
		}

		switch (World->GetNetMode())
		{
		case NM_Standalone:
			return TEXT("Standalone");
		case NM_DedicatedServer:
			return TEXT("DedicatedServer");
		case NM_ListenServer:
			return TEXT("ListenServer");
		case NM_Client:
			return TEXT("Client");
		default:
			return TEXT("UnknownNetMode");
		}
	}
}

void ADroneGamePlayerController::HostListen()
{
	const FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(this, true);
	if (CurrentLevelName.IsEmpty())
	{
		ShowNetworkMessage(TEXT("HostListen failed: current level name is empty."), FColor::Red);
		return;
	}

	ShowNetworkMessage(FString::Printf(TEXT("Hosting listen server on map %s"), *CurrentLevelName), FColor::Green);
	UGameplayStatics::OpenLevel(this, FName(*CurrentLevelName), true, TEXT("listen"));
}

void ADroneGamePlayerController::HostListenOnPort(int32 Port)
{
	const FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(this, true);
	if (CurrentLevelName.IsEmpty())
	{
		ShowNetworkMessage(TEXT("HostListenOnPort failed: current level name is empty."), FColor::Red);
		return;
	}

	const FString TravelOptions = FString::Printf(TEXT("listen?port=%d"), Port);
	ShowNetworkMessage(
		FString::Printf(TEXT("Hosting listen server on map %s using port %d"), *CurrentLevelName, Port),
		FColor::Green);
	UGameplayStatics::OpenLevel(this, FName(*CurrentLevelName), true, TravelOptions);
}

void ADroneGamePlayerController::JoinServer(const FString& Address)
{
	if (Address.IsEmpty())
	{
		ShowNetworkMessage(TEXT("JoinServer failed: address is empty. Example: JoinServer 192.168.1.10:7777"), FColor::Red);
		return;
	}

	FString NormalizedAddress = Address;
	if (!NormalizedAddress.Contains(TEXT(":")))
	{
		NormalizedAddress.Append(TEXT(":7777"));
	}

	ShowNetworkMessage(FString::Printf(TEXT("Attempting to join server at %s"), *NormalizedAddress), FColor::Yellow);
	ClientTravel(NormalizedAddress, TRAVEL_Absolute);
}

void ADroneGamePlayerController::NetworkStatus()
{
	const UWorld* World = GetWorld();
	const FString PawnName = GetPawn() ? GetPawn()->GetName() : FString(TEXT("NoPawn"));
	const FString UrlString = World ? World->URL.ToString() : FString(TEXT("NoURL"));

	ShowNetworkMessage(
		FString::Printf(
			TEXT("NetworkStatus | Local=%s | NetMode=%s | Pawn=%s | Map=%s | URL=%s"),
			IsLocalController() ? TEXT("true") : TEXT("false"),
			*DroneGamePlayerControllerDebug::GetNetModeString(World),
			*PawnName,
			World ? *World->GetMapName() : TEXT("NoMap"),
			*UrlString),
		FColor::Cyan,
		8.f);
}

void ADroneGamePlayerController::StartDeath()
{
	// Disabled in simplified drone-only mode.
}

bool ADroneGamePlayerController::CanRespawnPlayer() const
{
	return true;
}

float ADroneGamePlayerController::GetDeathTimerRemaining() const
{
	return 0.f;
}

bool ADroneGamePlayerController::IsDeathTimerActive() const
{
	return false;
}

void ADroneGamePlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Only initialize UI for local player controllers.
	if (!IsLocalController())
	{
		return;
	}

	if (PlayerHUDWidgetClass)
	{
		PlayerHUDWidget = CreateWidget<UUserWidget>(this, PlayerHUDWidgetClass);
		if (PlayerHUDWidget)
		{
			PlayerHUDWidget->AddToViewport();
		}
		else
		{
			UE_LOG(LogDroneGamePlayerController, Warning, TEXT("Failed to create PlayerHUDWidget for %s"), *GetName());
		}
	}
	else
	{
		UE_LOG(LogDroneGamePlayerController, Warning, TEXT("PlayerHUDWidgetClass is null in %s"), *GetName());
	}

	ShowNetworkMessage(
		FString::Printf(
			TEXT("Controller ready. NetMode=%s. Use HostListen / HostListenOnPort 7777 / JoinServer 192.168.x.x:7777 / NetworkStatus"),
			*DroneGamePlayerControllerDebug::GetNetModeString(GetWorld())),
		FColor::Green,
		8.f);
}

void ADroneGamePlayerController::OnDeathTimerFinished()
{
	// Disabled in simplified drone-only mode.
}

bool ADroneGamePlayerController::ReloadLevel()
{
	UGameplayStatics::OpenLevel(this,FName(UGameplayStatics::GetCurrentLevelName(this)));
	return true;
}

void ADroneGamePlayerController::ShowNetworkMessage(const FString& Message, const FColor& Color, float Duration) const
{
	if (const UDroneGameInstance* DroneGameInstance = Cast<UDroneGameInstance>(GetGameInstance()))
	{
		DroneGameInstance->ShowNetworkMessage(Message, Color, Duration);
		return;
	}

	UE_LOG(LogDroneGamePlayerController, Warning, TEXT("%s"), *Message);
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, Duration, Color, Message);
	}
}
