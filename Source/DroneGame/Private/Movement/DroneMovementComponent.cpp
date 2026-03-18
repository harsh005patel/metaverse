// Fill out your copyright notice in the Description page of Project Settings.

#include "Movement/DroneMovementComponent.h"

#include "DroneGameLogs.h"
#include "GameFramework/Pawn.h"
#include "PhysicsEngine/PhysicsThrusterComponent.h"

UDroneMovementComponent::UDroneMovementComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UDroneMovementComponent::AddDroneMovementInput(const FVector& Value)
{
	MoveInput = Value;
}

void UDroneMovementComponent::AddDroneLookInput(const FVector& Value)
{
	LookInput = Value;
}

void UDroneMovementComponent::AddDroneThrottle(float Value)
{
	ThrottleInput += Value;
}

void UDroneMovementComponent::SetComponentToMove(UPrimitiveComponent* InComp)
{
	if (!InComp)
	{
		DEBUG_LOG("ComponentToMove is null");
		return;
	}

	if (InComp->IsSimulatingPhysics())
	{
		InComp->SetSimulatePhysics(false);
	}

	ComponentToMove = InComp;
	PreviousLocation = ComponentToMove->GetComponentLocation();
	CurrentVelocity = FVector::ZeroVector;
	CurrentYaw = ComponentToMove->GetComponentRotation().Yaw;
	CurrentYawVelocity = 0.f;
}

void UDroneMovementComponent::SetThrusterComponent(UPhysicsThrusterComponent* InComp)
{
	if (!InComp)
	{
		return;
	}

	InComp->ThrustStrength = 0.f;
	if (InComp->IsActive())
	{
		InComp->Deactivate();
	}

	ThrusterComponent = InComp;
}

float UDroneMovementComponent::GetThrottlePercent() const
{
	const float Range = MaxThrottle - MinThrottle;
	if (Range == 0.f)
	{
		return 0.f;	
	}
	
	return (CurrentThrottle - MinThrottle) / Range;
}

float UDroneMovementComponent::GetVelocityLength() const
{
	return CachedVelocity.Length();
}

FVector UDroneMovementComponent::GetVelocity() const
{
	return CachedVelocity;
}

bool UDroneMovementComponent::IsVelocityDown() const
{
	return (CachedVelocity | FVector::DownVector) > 0.f;
}

void UDroneMovementComponent::DisableAllMovement()
{
	bEnableMovement = false;
	CurrentThrottle = 0.f;
	CachedVelocity = FVector::ZeroVector;
	CurrentVelocity = FVector::ZeroVector;
	CurrentYawVelocity = 0.f;
	if (ThrusterComponent)
	{
		ThrusterComponent->ThrustStrength = 0.f;
	}
}

void UDroneMovementComponent::EnableMovement()
{
	bEnableMovement = true;
}

void UDroneMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	const APawn* OwningPawn = GetPawn<APawn>();
	check(OwningPawn);
	
	UPrimitiveComponent* RootComp = Cast<UPrimitiveComponent>(OwningPawn->GetRootComponent());
	if (RootComp)
	{
		SetComponentToMove(RootComp);
	}
	else
	{
		DEBUG_LOG("UPrimitiveComponent is null");
	}
	
	UPhysicsThrusterComponent* ThrusterComp = OwningPawn->FindComponentByClass<UPhysicsThrusterComponent>();
	if (ThrusterComp)
	{
		SetThrusterComponent(ThrusterComp);
	}
	else
	{
		DEBUG_LOG("UPhysicsThrusterComponent is null");
	}

	
}

void UDroneMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                            FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	const APawn* OwningPawn = GetPawn<APawn>();
	if (OwningPawn && !OwningPawn->HasAuthority() && !OwningPawn->IsLocallyControlled())
	{
		const FVector NewLocation = ComponentToMove ? ComponentToMove->GetComponentLocation() : PreviousLocation;
		CachedVelocity = DeltaTime > 0.f ? (NewLocation - PreviousLocation) / DeltaTime : FVector::ZeroVector;
		PreviousLocation = NewLocation;
		return;
	}

	if (!bEnableMovement)
	{
		ThrottleInput = 0.f;
		MoveInput = FVector::ZeroVector;
		LookInput = FVector::ZeroVector;
		CachedVelocity = FVector::ZeroVector;
		CurrentVelocity = FVector::ZeroVector;
		CurrentYawVelocity = 0.f;
		return;
	}
	
	if (!ComponentToMove)
	{
		DEBUG_LOG("UDroneMovementComponent requires ComponentToMove");
		return;
	}

	CurrentThrottle += ThrottleInput * ThrottleSpeed * DeltaTime;
	CurrentThrottle = FMath::Clamp(CurrentThrottle, MinThrottle, MaxThrottle);

	const FVector PlanarInput = FVector(MoveInput.X, MoveInput.Y, 0.f).GetClampedToMaxSize(1.f);
	const float YawInput = FMath::Clamp(
		LookInput.Z != 0.f ? LookInput.Z : (LookInput.X != 0.f ? LookInput.X : MoveInput.Z),
		-1.f,
		1.f);

	const FRotator CurrentRotation = ComponentToMove->GetComponentRotation();
	const float TargetPitch = FMath::Clamp(-PlanarInput.X * MaxTiltAngle, -MaxTiltAngle, MaxTiltAngle);
	const float TargetRoll = FMath::Clamp(PlanarInput.Y * MaxTiltAngle, -MaxTiltAngle, MaxTiltAngle);
	const float TargetYawVelocity = YawInput * YawSpeed;

	CurrentYawVelocity = FMath::FInterpTo(CurrentYawVelocity, TargetYawVelocity, DeltaTime, YawAccelerationSpeed);
	CurrentYaw += CurrentYawVelocity * DeltaTime;

	const float NewPitch = FMath::FInterpTo(CurrentRotation.Pitch, TargetPitch, DeltaTime, AutoLevelSpeed);
	const float NewRoll = FMath::FInterpTo(CurrentRotation.Roll, TargetRoll, DeltaTime, AutoLevelSpeed);
	const FRotator NewRotation(
		FMath::ClampAngle(NewPitch, -MaxTiltAngle, MaxTiltAngle),
		CurrentYaw,
		FMath::ClampAngle(NewRoll, -MaxTiltAngle, MaxTiltAngle));

	const FRotator YawOnlyRotation(0.f, CurrentYaw, 0.f);
	const FVector ForwardDirection = FRotationMatrix(YawOnlyRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawOnlyRotation).GetUnitAxis(EAxis::Y);
	const float NormalizedThrottle = FMath::GetMappedRangeValueClamped(
		FVector2D(MinThrottle, MaxThrottle),
		FVector2D(-1.f, 1.f),
		CurrentThrottle);
	const FVector TargetVelocity =
		((ForwardDirection * PlanarInput.X) + (RightDirection * PlanarInput.Y)) * HorizontalSpeed;
	const FVector CurrentPlanarVelocity(CurrentVelocity.X, CurrentVelocity.Y, 0.f);
	const FVector SmoothedPlanarVelocity = FMath::VInterpTo(
		CurrentPlanarVelocity,
		TargetVelocity,
		DeltaTime,
		HorizontalAccelerationSpeed);

	CurrentVelocity.X = SmoothedPlanarVelocity.X * VelocityDamping;
	CurrentVelocity.Y = SmoothedPlanarVelocity.Y * VelocityDamping;

	const float TargetVerticalVelocity = NormalizedThrottle * VerticalSpeed;
	CurrentVelocity.Z = FMath::FInterpTo(
		CurrentVelocity.Z,
		TargetVerticalVelocity,
		DeltaTime,
		VerticalAccelerationSpeed);

	const FVector MoveDelta = CurrentVelocity * DeltaTime;

	FHitResult Hit;
	ComponentToMove->MoveComponent(MoveDelta, NewRotation.Quaternion(), true, &Hit);

	const FVector NewLocation = ComponentToMove->GetComponentLocation();
	CachedVelocity = DeltaTime > 0.f ? (NewLocation - PreviousLocation) / DeltaTime : FVector::ZeroVector;
	CurrentVelocity = CachedVelocity;
	PreviousLocation = NewLocation;

	if (ThrusterComponent)
	{
		ThrusterComponent->ThrustStrength = 0.f;
	}

	ThrottleInput = 0.f;
	MoveInput = FVector::ZeroVector;
	LookInput = FVector::ZeroVector;
}
