// Fill out your copyright notice in the Description page of Project Settings.

#include "Race/DroneRaceCheckpoint.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "DrawDebugHelpers.h"
#include "GameModes/DroneGameGameModeBase.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "UObject/ConstructorHelpers.h"

ADroneRaceCheckpoint::ADroneRaceCheckpoint()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetupAttachment(RootComponent);
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetCollisionObjectType(ECC_WorldDynamic);
	TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	TriggerBox->SetGenerateOverlapEvents(true);

	GateVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GateVisual"));
	GateVisual->SetupAttachment(RootComponent);
	GateVisual->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GateVisual->SetGenerateOverlapEvents(false);
	GateVisual->SetCanEverAffectNavigation(false);
	GateVisual->SetRelativeRotation(FRotator(90.f, 0.f, 0.f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMesh(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	if (CylinderMesh.Succeeded())
	{
		GateVisual->SetStaticMesh(CylinderMesh.Object);
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> TranslucentMaterial(TEXT("/Engine/EngineDebugMaterials/M_SimpleUnlitTranslucent.M_SimpleUnlitTranslucent"));
	if (TranslucentMaterial.Succeeded())
	{
		GateVisual->SetMaterial(0, TranslucentMaterial.Object);
	}

	LabelText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("LabelText"));
	LabelText->SetupAttachment(RootComponent);
	LabelText->SetHorizontalAlignment(EHTA_Center);
	LabelText->SetVerticalAlignment(EVRTA_TextCenter);
	LabelText->SetWorldSize(140.f);
	LabelText->SetTextRenderColor(GateColor);
	LabelText->SetRelativeLocation(FVector(0.f, 0.f, 430.f));
	LabelText->SetRelativeRotation(FRotator(0.f, 180.f, 0.f));
}

void ADroneRaceCheckpoint::BeginPlay()
{
	Super::BeginPlay();

	if (TriggerBox)
	{
		TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnGateOverlap);
	}

	RefreshGateVisuals();
}

void ADroneRaceCheckpoint::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	RefreshGateVisuals();
}

void ADroneRaceCheckpoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bDrawDebugGate)
	{
		DrawGate();
	}
}

void ADroneRaceCheckpoint::SetCheckpointIndex(int32 NewCheckpointIndex)
{
	CheckpointIndex = FMath::Max(1, NewCheckpointIndex);
	RefreshGateVisuals();
}

void ADroneRaceCheckpoint::OnGateOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (!HasAuthority())
	{
		return;
	}

	APawn* Pawn = Cast<APawn>(OtherActor);
	if (!Pawn || !Pawn->GetController())
	{
		return;
	}

	if (ADroneGameGameModeBase* GameMode = GetWorld()->GetAuthGameMode<ADroneGameGameModeBase>())
	{
		GameMode->HandleCheckpointReached(this, Pawn);
	}
}

void ADroneRaceCheckpoint::RefreshGateVisuals()
{
	if (TriggerBox)
	{
		TriggerBox->SetBoxExtent(GateExtent);
	}

	if (GateVisual)
	{
		const float Radius = FMath::Max(GateExtent.Y, GateExtent.Z);
		GateVisual->SetVisibility(bShowSolidGate);
		GateVisual->SetRelativeScale3D(FVector(
			Radius / 50.f,
			Radius / 50.f,
			FMath::Max(1.f, GateExtent.X) / 50.f));
	}

	if (LabelText)
	{
		LabelText->SetText(FText::FromString(FString::Printf(TEXT("CP %d"), CheckpointIndex)));
		LabelText->SetTextRenderColor(GateColor);
		LabelText->SetRelativeLocation(FVector(0.f, 0.f, FMath::Max(GateExtent.Y, GateExtent.Z) + 90.f));
	}

	RefreshGateMaterial();
}

void ADroneRaceCheckpoint::RefreshGateMaterial()
{
	if (!GateVisual)
	{
		return;
	}

	GateMaterialInstance = Cast<UMaterialInstanceDynamic>(GateVisual->GetMaterial(0));
	if (!GateMaterialInstance)
	{
		GateMaterialInstance = GateVisual->CreateDynamicMaterialInstance(0);
	}

	if (!GateMaterialInstance)
	{
		return;
	}

	const FLinearColor LinearGateColor = FLinearColor(GateColor).CopyWithNewOpacity(FMath::Clamp(GateOpacity, 0.f, 1.f));
	GateMaterialInstance->SetVectorParameterValue(TEXT("Color"), LinearGateColor);
	GateMaterialInstance->SetVectorParameterValue(TEXT("BaseColor"), LinearGateColor);
	GateMaterialInstance->SetVectorParameterValue(TEXT("EmissiveColor"), LinearGateColor);
	GateMaterialInstance->SetScalarParameterValue(TEXT("Opacity"), LinearGateColor.A);
	GateMaterialInstance->SetScalarParameterValue(TEXT("Alpha"), LinearGateColor.A);
}

void ADroneRaceCheckpoint::DrawGate() const
{
	const UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	DrawDebugBox(World, GetActorLocation(), GateExtent, GetActorQuat(), GateColor, false, 0.f, 0, GateDrawThickness);
}
