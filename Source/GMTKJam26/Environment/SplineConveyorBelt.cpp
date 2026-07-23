#include "SplineConveyorBelt.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Interaction/InteractableItem.h"

ASplineConveyorBelt::ASplineConveyorBelt()
{
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	ConveyorSpline = CreateDefaultSubobject<USplineComponent>(TEXT("ConveyorSpline"));
	ConveyorSpline->SetupAttachment(RootComponent);
	ConveyorSpline->ClearSplinePoints();
	ConveyorSpline->AddSplinePoint(FVector(0.f, 0.f, 0.f), ESplineCoordinateSpace::Local, false);
	ConveyorSpline->AddSplinePoint(FVector(500.f, 0.f, 0.f), ESplineCoordinateSpace::Local, false);
	ConveyorSpline->UpdateSpline();

	CarryZone = CreateDefaultSubobject<UBoxComponent>(TEXT("CarryZone"));
	CarryZone->SetupAttachment(RootComponent);
	CarryZone->SetCollisionProfileName(FName("OverlapAllDynamic"));

	SpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("SpawnPoint"));
	SpawnPoint->SetupAttachment(RootComponent);
	SpawnPoint->SetRelativeLocation(FVector(0.f, 0.f, 50.f));
}

void ASplineConveyorBelt::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	RebuildCarryZone();
	RebuildVisualSegments();
}

void ASplineConveyorBelt::BeginPlay()
{
	Super::BeginPlay();

	RebuildCarryZone();

	if (bSpawningEnabled && ItemsToSpawn.Num() > 0)
	{
		ScheduleNextSpawn();
	}
}

void ASplineConveyorBelt::RebuildCarryZone()
{
	if (!ConveyorSpline || !CarryZone)
	{
		return;
	}

	const float SplineLength = ConveyorSpline->GetSplineLength();
	if (SplineLength <= 0.f)
	{
		return;
	}

	FVector MinPoint(TNumericLimits<float>::Max());
	FVector MaxPoint(TNumericLimits<float>::Lowest());

	const int32 NumSamples = FMath::Max(FMath::CeilToInt(SplineLength / 50.f), 2);
	for (int32 Index = 0; Index <= NumSamples; ++Index)
	{
		const float Distance = (SplineLength * Index) / NumSamples;
		const FVector Point = ConveyorSpline->GetLocationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::Local);
		MinPoint = FVector::Min(MinPoint, Point);
		MaxPoint = FVector::Max(MaxPoint, Point);
	}

	const FVector Center = (MinPoint + MaxPoint) * 0.5f;
	const FVector HalfExtent = (MaxPoint - MinPoint) * 0.5f + FVector(MaxRideDistance);

	CarryZone->SetRelativeLocation(Center);
	CarryZone->SetBoxExtent(HalfExtent.ComponentMax(FVector(10.f)));
}

void ASplineConveyorBelt::RebuildVisualSegments()
{
	for (USplineMeshComponent* Segment : VisualSegments)
	{
		if (Segment)
		{
			Segment->DestroyComponent();
		}
	}
	VisualSegments.Empty();

	if (!BeltSegmentMesh || !ConveyorSpline)
	{
		return;
	}

	const float SplineLength = ConveyorSpline->GetSplineLength();
	const int32 NumSegments = FMath::Max(FMath::FloorToInt(SplineLength / SegmentLength), 1);

	for (int32 Index = 0; Index < NumSegments; ++Index)
	{
		const float StartDistance = Index * SegmentLength;
		const float EndDistance = FMath::Min(StartDistance + SegmentLength, SplineLength);

		USplineMeshComponent* Segment = NewObject<USplineMeshComponent>(this, NAME_None, RF_Transactional);
		Segment->SetMobility(EComponentMobility::Movable);
		Segment->SetStaticMesh(BeltSegmentMesh);
		Segment->SetupAttachment(ConveyorSpline);
		Segment->RegisterComponentWithWorld(GetWorld());

		Segment->SetCollisionProfileName(FName("BlockAll"));

		const FVector StartPos = ConveyorSpline->GetLocationAtDistanceAlongSpline(StartDistance, ESplineCoordinateSpace::Local);
		const FVector StartTangent = ConveyorSpline->GetTangentAtDistanceAlongSpline(StartDistance, ESplineCoordinateSpace::Local);
		const FVector EndPos = ConveyorSpline->GetLocationAtDistanceAlongSpline(EndDistance, ESplineCoordinateSpace::Local);
		const FVector EndTangent = ConveyorSpline->GetTangentAtDistanceAlongSpline(EndDistance, ESplineCoordinateSpace::Local);

		Segment->SetStartAndEnd(StartPos, StartTangent, EndPos, EndTangent);

		VisualSegments.Add(Segment);
	}
}

void ASplineConveyorBelt::EnableSpawning()
{
	if (bSpawningEnabled)
	{
		return;
	}

	bSpawningEnabled = true;

	if (ItemsToSpawn.Num() > 0 && !GetWorldTimerManager().IsTimerActive(SpawnTimerHandle))
	{
		ScheduleNextSpawn();
	}
}

void ASplineConveyorBelt::DisableSpawning()
{
	bSpawningEnabled = false;
	GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
}

void ASplineConveyorBelt::ScheduleNextSpawn()
{
	const float Interval = FMath::FRandRange(FMath::Max(MinSpawnInterval, 0.f), FMath::Max(MaxSpawnInterval, MinSpawnInterval));
	GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &ASplineConveyorBelt::SpawnItem, Interval, false);
}

void ASplineConveyorBelt::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!ConveyorSpline || !CarryZone)
	{
		return;
	}

	const float SplineLength = ConveyorSpline->GetSplineLength();
	if (SplineLength <= 0.f)
	{
		return;
	}

	TArray<AActor*> OverlappingActors;
	CarryZone->GetOverlappingActors(OverlappingActors);

	TSet<AActor*> StillOverlapping;
	StillOverlapping.Reserve(OverlappingActors.Num());

	for (AActor* Actor : OverlappingActors)
	{
		if (!Actor || Actor == this)
		{
			continue;
		}

		StillOverlapping.Add(Actor);

		const FVector ActorLocation = Actor->GetActorLocation();
		const TWeakObjectPtr<AActor> ActorKey(Actor);
		UPrimitiveComponent* RootPrimitive = Cast<UPrimitiveComponent>(Actor->GetRootComponent());
		const bool bIsPhysicsItem = RootPrimitive && RootPrimitive->IsSimulatingPhysics();

		float CurrentDistance;
		if (const float* ExistingDistance = RidingProgress.Find(ActorKey))
		{
			CurrentDistance = *ExistingDistance;

			const FVector ExpectedPoint = ConveyorSpline->GetLocationAtDistanceAlongSpline(CurrentDistance, ESplineCoordinateSpace::World);
			if (FVector::Dist(ActorLocation, ExpectedPoint) > MaxRideDistance)
			{
				if (bIsPhysicsItem)
				{
					RootPrimitive->SetEnableGravity(true);
				}
				RidingProgress.Remove(ActorKey);
				continue;
			}
		}
		else
		{
			const float InputKey = ConveyorSpline->FindInputKeyClosestToWorldLocation(ActorLocation);
			const FVector ClosestPoint = ConveyorSpline->GetLocationAtSplineInputKey(InputKey, ESplineCoordinateSpace::World);

			if (FVector::Dist(ActorLocation, ClosestPoint) > MaxRideDistance)
			{
				continue;
			}

			CurrentDistance = ConveyorSpline->GetDistanceAlongSplineAtSplineInputKey(InputKey);
		}

		if (CurrentDistance >= SplineLength)
		{
			if (bIsPhysicsItem)
			{
				const FVector ExitDirection = ConveyorSpline->GetDirectionAtDistanceAlongSpline(SplineLength, ESplineCoordinateSpace::World).GetSafeNormal();
				RootPrimitive->SetPhysicsLinearVelocity(ExitDirection * ConveyorSpeed);
				RootPrimitive->SetEnableGravity(true);
			}

			RidingProgress.Remove(ActorKey);
			continue;
		}

		const FVector ClosestPointNow = ConveyorSpline->GetLocationAtDistanceAlongSpline(CurrentDistance, ESplineCoordinateSpace::World);
		const FVector LateralOffset = ActorLocation - ClosestPointNow;

		const float NewDistance = FMath::Min(CurrentDistance + ConveyorSpeed * DeltaTime, SplineLength);
		const FVector NewSplinePoint = ConveyorSpline->GetLocationAtDistanceAlongSpline(NewDistance, ESplineCoordinateSpace::World);
		const FVector TargetLocation = NewSplinePoint + LateralOffset;

		if (bIsPhysicsItem)
		{
			const FVector SplineDirection = ConveyorSpline->GetDirectionAtDistanceAlongSpline(NewDistance, ESplineCoordinateSpace::World);
			const float PitchDegrees = FMath::RadiansToDegrees(FMath::Asin(FMath::Clamp(SplineDirection.Z, -1.f, 1.f)));
			const bool bSteepUphill = PitchDegrees > UphillGravityDisableThresholdDegrees;

			RootPrimitive->SetEnableGravity(!bSteepUphill);

			const FVector CurrentAngularVelocity = RootPrimitive->GetPhysicsAngularVelocityInDegrees();
			const FVector NewAngularVelocity = FMath::VInterpConstantTo(CurrentAngularVelocity, FVector::ZeroVector, DeltaTime, AngularDampingRate);
			RootPrimitive->SetPhysicsAngularVelocityInDegrees(NewAngularVelocity);
		}

		Actor->SetActorLocation(TargetLocation, false);

		RidingProgress.Add(ActorKey, NewDistance);
	}

	for (auto It = RidingProgress.CreateIterator(); It; ++It)
	{
		if (!It->Key.IsValid() || !StillOverlapping.Contains(It->Key.Get()))
		{
			if (AActor* LeftActor = It->Key.Get())
			{
				if (UPrimitiveComponent* LeftPrimitive = Cast<UPrimitiveComponent>(LeftActor->GetRootComponent()))
				{
					if (LeftPrimitive->IsSimulatingPhysics())
					{
						LeftPrimitive->SetEnableGravity(true);
					}
				}
			}

			It.RemoveCurrent();
		}
	}
}

void ASplineConveyorBelt::SpawnItem()
{
	TSubclassOf<AInteractableItem> ChosenClass = PickRandomItemClass();
	if (ChosenClass && SpawnPoint)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		if (AInteractableItem* SpawnedItem = GetWorld()->SpawnActor<AInteractableItem>(ChosenClass, SpawnPoint->GetComponentLocation(), SpawnPoint->GetComponentRotation(), SpawnParams))
		{
			OnItemSpawned.Broadcast(SpawnedItem);
		}
	}

	if (bSpawningEnabled && ItemsToSpawn.Num() > 0)
	{
		ScheduleNextSpawn();
	}
}

TSubclassOf<AInteractableItem> ASplineConveyorBelt::PickRandomItemClass() const
{
	float TotalWeight = 0.f;
	for (const FConveyorSpawnEntry& Entry : ItemsToSpawn)
	{
		if (Entry.ItemClass)
		{
			TotalWeight += FMath::Max(Entry.SpawnWeight, 0.f);
		}
	}

	if (TotalWeight <= 0.f)
	{
		return nullptr;
	}

	float Roll = FMath::FRandRange(0.f, TotalWeight);
	for (const FConveyorSpawnEntry& Entry : ItemsToSpawn)
	{
		if (!Entry.ItemClass)
		{
			continue;
		}

		Roll -= FMath::Max(Entry.SpawnWeight, 0.f);
		if (Roll <= 0.f)
		{
			return Entry.ItemClass;
		}
	}

	return ItemsToSpawn.Last().ItemClass;
}
