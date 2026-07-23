#include "PlayerTimerComponent.h"
#include "Interaction/RobotPart.h"

UPlayerTimerComponent::UPlayerTimerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;
}

void UPlayerTimerComponent::InitializeComponent()
{
	Super::InitializeComponent();

	Segments.Empty();

	for (const TSubclassOf<ARobotPart>& ItemClass : ItemOrder)
	{
		FTimerSegment Segment;
		Segment.ItemClass = ItemClass;
		Segments.Add(Segment);
	}

	FTimerSegment CoreSegment;
	CoreSegment.ItemClass = nullptr;
	CoreSegment.MaxDuration = CoreDuration;
	CoreSegment.RemainingTime = CoreDuration;
	CoreSegment.bIsDelivered = true;
	Segments.Add(CoreSegment);
}

void UPlayerTimerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bExpired)
	{
		return;
	}

	const int32 ActiveIndex = FindActiveSegmentIndex();
	if (ActiveIndex == INDEX_NONE)
	{
		bExpired = true;
		OnTimerExpired.Broadcast();
		return;
	}

	FTimerSegment& ActiveSegment = Segments[ActiveIndex];
	ActiveSegment.RemainingTime = FMath::Max(ActiveSegment.RemainingTime - DeltaTime, 0.f);

	if (ActiveSegment.RemainingTime <= 0.f)
	{
		OnSegmentDepleted.Broadcast(ActiveIndex);
	}
}

bool UPlayerTimerComponent::AddItem(ARobotPart* Item)
{
	if (!Item)
	{
		return false;
	}

	const TSubclassOf<ARobotPart> ItemClass = Item->GetClass();

	for (FTimerSegment& Segment : Segments)
	{
		if (Segment.ItemClass != ItemClass)
		{
			continue;
		}

		if (Segment.bIsDelivered)
		{
			return false;
		}

		Segment.MaxDuration = Item->GetSeconds();
		Segment.RemainingTime = Item->GetSeconds();
		Segment.bIsDelivered = true;

		OnItemAdded.Broadcast(ItemClass);
		return true;
	}

	return false;
}

void UPlayerTimerComponent::ForfeitActiveSegment()
{
	const int32 ActiveIndex = FindActiveSegmentIndex();
	if (ActiveIndex == INDEX_NONE)
	{
		return;
	}

	FTimerSegment& ActiveSegment = Segments[ActiveIndex];
	if (!ActiveSegment.ItemClass)
	{
		return;
	}

	ActiveSegment.RemainingTime = 0.f;
	OnSegmentDepleted.Broadcast(ActiveIndex);
}

float UPlayerTimerComponent::GetTotalRemainingTime() const
{
	float Total = 0.f;
	for (const FTimerSegment& Segment : Segments)
	{
		if (Segment.bIsDelivered)
		{
			Total += Segment.RemainingTime;
		}
	}

	return Total;
}

int32 UPlayerTimerComponent::FindActiveSegmentIndex() const
{
	for (int32 Index = 0; Index < Segments.Num(); ++Index)
	{
		if (Segments[Index].bIsDelivered && Segments[Index].RemainingTime > 0.f)
		{
			return Index;
		}
	}

	return INDEX_NONE;
}
