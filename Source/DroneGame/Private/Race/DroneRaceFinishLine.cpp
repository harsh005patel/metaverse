// Fill out your copyright notice in the Description page of Project Settings.

#include "Race/DroneRaceFinishLine.h"

#include "Components/BoxComponent.h"
#include "Components/TextRenderComponent.h"
#include "DrawDebugHelpers.h"
#include "GameModes/DroneGameGameModeBase.h"

ADroneRaceFinishLine::ADroneRaceFinishLine()
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

	LabelText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("LabelText"));
	LabelText->SetupAttachment(RootComponent);
	LabelText->SetHorizontalAlignment(EHTA_Center);
	LabelText->SetVerticalAlignment(EVRTA_TextCenter);
	LabelText->SetWorldSize(150.f);
	LabelText->SetTextRenderColor(GateColor);
	LabelText->SetRelativeLocation(FVector(0.f, 0.f, 500.f));
	LabelText->SetRelativeRotation(FRotator(0.f, 180.f, 0.f));
}

void ADroneRaceFinishLine::BeginPlay()
{
	Super::BeginPlay();

	if (TriggerBox)
	{
		TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnGateOverlap);
	}

	RefreshGateVisuals();
}

void ADroneRaceFinishLine::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	RefreshGateVisuals();
}

void ADroneRaceFinishLine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bDrawGate)
	{
		DrawGate();
	}
}

void ADroneRaceFinishLine::OnGateOverlap(
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
		GameMode->HandleFinishLineReached(this, Pawn);
	}
}

void ADroneRaceFinishLine::RefreshGateVisuals()
{
	if (TriggerBox)
	{
		TriggerBox->SetBoxExtent(GateExtent);
	}

	if (LabelText)
	{
		LabelText->SetText(FText::FromString(TEXT("FINISH")));
		LabelText->SetTextRenderColor(GateColor);
		LabelText->SetRelativeLocation(FVector(0.f, 0.f, GateExtent.Z + 100.f));
	}
}

void ADroneRaceFinishLine::DrawGate() const
{
	const UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	DrawDebugBox(World, GetActorLocation(), GateExtent, GetActorQuat(), GateColor, false, 0.f, 0, GateDrawThickness);
}
