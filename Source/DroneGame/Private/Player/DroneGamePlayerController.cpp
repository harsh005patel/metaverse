// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/DroneGamePlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY_STATIC(LogDroneGamePlayerController, Log, All);

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
