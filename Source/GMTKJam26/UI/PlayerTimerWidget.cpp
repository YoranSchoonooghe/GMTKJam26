#include "PlayerTimerWidget.h"
#include "TimerSegmentWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/PlayerTimerComponent.h"
#include "Engine/Texture2D.h"

void UPlayerTimerWidget::InitTimerComponent(UPlayerTimerComponent* InTimerComponent)
{
	TimerComponent = InTimerComponent;
}

void UPlayerTimerWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	BuildSegmentWidgets();
}

void UPlayerTimerWidget::BuildSegmentWidgets()
{
	SegmentWidgets.Empty();

	if (!SegmentContainer || !SegmentWidgetClass)
	{
		return;
	}

	SegmentContainer->ClearChildren();

	TArray<FTimerSegmentVisual> AllVisuals = ItemVisuals;
	AllVisuals.Add(CoreVisual);

	float NextX = 0.f;

	for (const FTimerSegmentVisual& Visual : AllVisuals)
	{
		UTimerSegmentWidget* SegmentWidget = CreateWidget<UTimerSegmentWidget>(this, SegmentWidgetClass);
		if (!SegmentWidget)
		{
			continue;
		}

		SegmentWidget->SetVisual(Visual.DisplayName, Visual.FillIcon, Visual.BackgroundIcon, Visual.Offset);

		if (Visual.bMirrorIcon)
		{
			SegmentWidget->SetRenderTransformPivot(FVector2D(0.5f, 0.5f));
			SegmentWidget->SetRenderScale(FVector2D(-1.f, 1.f));
		}

		const UTexture2D* SizingTexture = Visual.FillIcon ? Visual.FillIcon : Visual.BackgroundIcon;
		const FVector2D NativeSize = SizingTexture ? FVector2D(SizingTexture->GetSizeX(), SizingTexture->GetSizeY()) : FVector2D(400.f, 300.f);
		const FVector2D SegmentSize = NativeSize * GlobalScale;

		if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(SegmentContainer->AddChild(SegmentWidget)))
		{
			CanvasSlot->SetAnchors(FAnchors(0.f, 0.f));
			CanvasSlot->SetAlignment(FVector2D::ZeroVector);
			CanvasSlot->SetAutoSize(false);
			CanvasSlot->SetSize(SegmentSize);
			CanvasSlot->SetPosition(FVector2D(NextX, 0.f));
		}

		NextX += SegmentSize.X - OverlapAmount;

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
			SegmentWidgets[Index]->UpdateSegment(Segments[Index], Index == ActiveIndex, InDeltaTime);
		}
	}
}
