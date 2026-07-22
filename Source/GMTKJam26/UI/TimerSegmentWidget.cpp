#include "TimerSegmentWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Engine/Texture2D.h"

void UTimerSegmentWidget::SetDisplayName(const FText& InDisplayName)
{
	if (NameLabel)
	{
		NameLabel->SetText(InDisplayName);
	}
}

void UTimerSegmentWidget::SetIcons(UTexture2D* FillIcon, UTexture2D* BackgroundIcon)
{
	if (!TimeBar)
	{
		return;
	}

	FProgressBarStyle NewStyle = TimeBar->WidgetStyle;

	if (FillIcon)
	{
		NewStyle.FillImage.SetResourceObject(FillIcon);
		NewStyle.FillImage.ImageSize = FVector2D(FillIcon->GetSizeX(), FillIcon->GetSizeY());
		NewStyle.FillImage.TintColor = FSlateColor(FLinearColor::White);
	}

	if (BackgroundIcon)
	{
		NewStyle.BackgroundImage.SetResourceObject(BackgroundIcon);
		NewStyle.BackgroundImage.ImageSize = FVector2D(BackgroundIcon->GetSizeX(), BackgroundIcon->GetSizeY());
		NewStyle.BackgroundImage.TintColor = FSlateColor(FLinearColor::White);
	}

	TimeBar->SetWidgetStyle(NewStyle);
}

void UTimerSegmentWidget::UpdateSegment(const FTimerSegment& Segment, bool bIsActive)
{
	const float Percent = Segment.MaxDuration > 0.f ? FMath::Clamp(Segment.RemainingTime / Segment.MaxDuration, 0.f, 1.f) : 0.f;

	if (TimeBar)
	{
		TimeBar->SetPercent(Percent);
	}

	if (TimeLabel)
	{
		TimeLabel->SetText(Segment.bIsDelivered ? FText::AsNumber(FMath::CeilToInt(Segment.RemainingTime)) : FText::FromString(TEXT("--")));
	}
}
