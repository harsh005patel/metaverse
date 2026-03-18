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

	CurrentMoveInput = Value;

	if (IsLocallyControlled())
	{
		DroneMovementComponent->AddDroneMovementInput(Value);
	}

	if (!HasAuthority())
	{
		SendControlStateToServer();
	}
}

void ADronePawnBase::HandleLookInput(const FVector& Value)
{
	if (!DroneMovementComponent)
	{
		return;
	}

	CurrentLookInput = Value;

	if (IsLocallyControlled())
	{
		DroneMovementComponent->AddDroneLookInput(Value);
	}

	if (!HasAuthority())
	{
		SendControlStateToServer();
	}
}

void ADronePawnBase::HandleThrottleInput(float Value)
{
	if (!DroneMovementComponent)
	{
		return;
	}

	CurrentThrottleInput = Value;

	if (IsLocallyControlled())
	{
		DroneMovementComponent->AddDroneThrottle(Value);
	}

	if (!HasAuthority())
	{
		SendControlStateToServer();
	}
}

void ADronePawnBase::SendControlStateToServer()
{
	ServerSetControlState(CurrentMoveInput, CurrentLookInput, CurrentThrottleInput);
}

void ADronePawnBase::ServerSetControlState_Implementation(const FVector& MoveValue, const FVector& LookValue,
	float ThrottleValue)
{
	if (DroneMovementComponent)
	{
		DroneMovementComponent->AddDroneMovementInput(MoveValue);
		DroneMovementComponent->AddDroneLookInput(LookValue);
		DroneMovementComponent->AddDroneThrottle(ThrottleValue);
	}
}
