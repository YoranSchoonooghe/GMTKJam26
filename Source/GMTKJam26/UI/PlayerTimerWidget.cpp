#include "PlayerTimerWidget.h"
#include "TimerSegmentWidget.h"
#include "Components/PanelWidget.h"
#include "Components/PlayerTimerComponent.h"

void UPlayerTimerWidget::InitTimerComponent(UPlayerTimerComponent* InTimerComponent)
{
	TimerComponent = InTimerComponent;
	BuildSegmentWidgets();
}

void UPlayerTimerWidget::BuildSegmentWidgets()
{
	SegmentWidgets.Empty();

	if (!SegmentContainer || !SegmentWidgetClass || !TimerComponent)
	{
		return;
	}

	SegmentContainer->ClearChildren();

	const TArray<FTimerSegment> Segments = TimerComponent->GetSegments();
	for (int32 Index = 0; Index < Segments.Num(); ++Index)
	{
		UTimerSegmentWidget* SegmentWidget = CreateWidget<UTimerSegmentWidget>(this, SegmentWidgetClass);
		if (!SegmentWidget)
		{
			continue;
		}

		const bool bIsCoreSegment = (Index == Segments.Num() - 1);

		FTimerSegmentVisual Visual;
		if (bIsCoreSegment)
		{
			Visual = CoreVisual;
		}
		else if (ItemVisuals.IsValidIndex(Index))
		{
			Visual = ItemVisuals[Index];
		}

		SegmentWidget->SetDisplayName(Visual.DisplayName);
		SegmentWidget->SetIcons(Visual.FillIcon, Visual.BackgroundIcon);
		SegmentContainer->AddChild(SegmentWidget);
		SegmentWidgets.Add(SegmentWidget);
	}
}

void UPlayerTimerWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!TimerComponent)
	{
		return;
	}

	const TArray<FTimerSegment> Segments = TimerComponent->GetSegments();
	const int32 ActiveIndex = TimerComponent->GetActiveSegmentIndex();

	for (int32 Index = 0; Index < SegmentWidgets.Num() && Index < Segments.Num(); ++Index)
	{
		if (SegmentWidgets[Index])
		{
			SegmentWidgets[Index]->UpdateSegment(Segments[Index], Index == ActiveIndex);
		}
	}
}
