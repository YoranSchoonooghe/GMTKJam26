#include "TimerSegmentWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Engine/Texture2D.h"
#include "Animation/WidgetAnimation.h"

void UTimerSegmentWidget::SetVisual(const FText& InDisplayName, UTexture2D* InFillIcon, UTexture2D* InBackgroundIcon, const FVector2D& InOffset, float InMinFillPercent, float InMaxFillPercent)
{
	DisplayName = InDisplayName;
	FillIcon = InFillIcon;
	BackgroundIcon = InBackgroundIcon;
	Offset = InOffset;
	MinFillPercent = InMinFillPercent;
	MaxFillPercent = InMaxFillPercent;

	ApplyVisual();
}

void UTimerSegmentWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	ApplyVisual();
}

void UTimerSegmentWidget::ApplyVisual()
{
	SetRenderTranslation(Offset);

	if (NameLabel)
	{
		NameLabel->SetText(DisplayName);
	}

	if (TimeBar)
	{
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
}

void UTimerSegmentWidget::UpdateSegment(const FTimerSegment& Segment, bool bIsActive, float DeltaTime)
{
	const float Percent = Segment.MaxDuration > 0.f ? FMath::Clamp(Segment.RemainingTime / Segment.MaxDuration, 0.f, 1.f) : 0.f;

	if (TimeBar)
	{
		TimeBar->SetPercent(FMath::Lerp(MinFillPercent, MaxFillPercent, Percent));
	}

	if (TimeLabel)
	{
		TimeLabel->SetText(Segment.bIsDelivered ? FText::AsNumber(FMath::CeilToInt(Segment.RemainingTime)) : FText::FromString(TEXT("--")));
	}

	if (bIsActive && !bWasActive && PopInAnim)
	{
		PlayAnimation(PopInAnim);
	}

	if (bHasPreviousRemainingTime && PopLostAnim && (PreviousRemainingTime - Segment.RemainingTime) > LostTimeJumpThreshold)
	{
		PlayAnimation(PopLostAnim);
	}

	const bool bShouldShake = bIsActive && Segment.bIsDelivered && LowTimeShakeThreshold > 0.f && Segment.RemainingTime < LowTimeShakeThreshold;

	if (bShouldShake)
	{
		const float ShakeAlpha = FMath::Clamp(1.f - (Segment.RemainingTime / LowTimeShakeThreshold), 0.f, 1.f);
		const float Frequency = FMath::Lerp(MinLowTimeShakeFrequency, MaxLowTimeShakeFrequency, ShakeAlpha);

		ShakePhase += DeltaTime * Frequency;
		const float ShakeAngle = FMath::Sin(ShakePhase) * MaxLowTimeShakeAngle * ShakeAlpha;

		SetRenderTransformAngle(ShakeAngle);
	}
	else if (bWasShaking)
	{
		ShakePhase = 0.f;
		SetRenderTransformAngle(0.f);
	}

	if (bIsActive)
	{
		PulsePhase += DeltaTime * ActivePulseSpeed;
		const float PulseScale = 1.f + FMath::Sin(PulsePhase) * ActivePulseScale;
		SetRenderScale(FVector2D(PulseScale, PulseScale));
	}
	else if (bWasActive)
	{
		PulsePhase = 0.f;
		SetRenderScale(FVector2D(1.f, 1.f));
	}

	bWasShaking = bShouldShake;
	bWasActive = bIsActive;
	PreviousRemainingTime = Segment.RemainingTime;
	bHasPreviousRemainingTime = true;
}
