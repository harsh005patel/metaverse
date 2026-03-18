// Fill out your copyright notice in the Description page of Project Settings.

#include "Pawns/DronePawnBase.h"

#include "Components/BoxComponent.h"
#include "Movement/DroneMovementComponent.h"
#include "PhysicsEngine/PhysicsThrusterComponent.h"

ADronePawnBase::ADronePawnBase()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	SetReplicateMovement(true);
	bAlwaysRelevant = true;
	NetUpdateFrequency = 100.f;
	MinNetUpdateFrequency = 33.f;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>("BoxComponent");
	RootComponent = BoxComponent;

	PhysicsThruster = CreateDefaultSubobject<UPhysicsThrusterComponent>("PhysicsThrusterComponent");
	PhysicsThruster->SetupAttachment(RootComponent);
	
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>("SkeletalMeshComponent");
	SkeletalMesh->SetupAttachment(RootComponent);

	DroneMovementComponent = CreateDefaultSubobject<UDroneMovementComponent>("DroneMovementComponent");
}

void ADronePawnBase::BeginPlay()
{
	Super::BeginPlay();
}

void ADronePawnBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADronePawnBase::HandleMoveInput(const FVector& Value)
{
	if (!DroneMovementComponent)
	{
		return;
	}

	if (IsLocallyControlled())
	{
		DroneMovementComponent->AddDroneMovementInput(Value);
	}

	if (!HasAuthority())
	{
		ServerHandleMoveInput(Value);
	}
}

void ADronePawnBase::HandleLookInput(const FVector& Value)
{
	if (!DroneMovementComponent)
	{
		return;
	}

	if (IsLocallyControlled())
	{
		DroneMovementComponent->AddDroneLookInput(Value);
	}

	if (!HasAuthority())
	{
		ServerHandleLookInput(Value);
	}
}

void ADronePawnBase::HandleThrottleInput(float Value)
{
	if (!DroneMovementComponent)
	{
		return;
	}

	if (IsLocallyControlled())
	{
		DroneMovementComponent->AddDroneThrottle(Value);
	}

	if (!HasAuthority())
	{
		ServerHandleThrottleInput(Value);
	}
}

void ADronePawnBase::ServerHandleMoveInput_Implementation(const FVector& Value)
{
	if (DroneMovementComponent)
	{
		DroneMovementComponent->AddDroneMovementInput(Value);
	}
}

void ADronePawnBase::ServerHandleLookInput_Implementation(const FVector& Value)
{
	if (DroneMovementComponent)
	{
		DroneMovementComponent->AddDroneLookInput(Value);
	}
}

void ADronePawnBase::ServerHandleThrottleInput_Implementation(float Value)
{
	if (DroneMovementComponent)
	{
		DroneMovementComponent->AddDroneThrottle(Value);
	}
}
