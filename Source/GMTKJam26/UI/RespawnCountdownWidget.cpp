#include "RespawnCountdownWidget.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

void URespawnCountdownWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetAlignmentInViewport(FVector2D(0.5f, 0.5f));
}

void URespawnCountdownWidget::StartCountdown(const FVector& InWorldLocation, float InDuration)
{
	WorldLocation = InWorldLocation;
	Duration = InDuration;
	Elapsed = 0.f;

	UpdateScreenPosition();
}

void URespawnCountdownWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	Elapsed += InDeltaTime;

	if (CountdownLabel)
	{
		const float Remaining = FMath::Max(Duration - Elapsed, 0.f);
		CountdownLabel->SetText(FText::AsNumber(FMath::CeilToInt(Remaining)));
	}
}

void URespawnCountdownWidget::UpdateScreenPosition()
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
	{
		FVector2D ScreenPosition;
		if (PC->ProjectWorldLocationToScreen(WorldLocation, ScreenPosition))
		{
			constexpr float margin{ 50.0f };
			int32 width, height;
			PC->GetViewportSize(width, height);

			ScreenPosition.X = FMath::Clamp(ScreenPosition.X, margin, width - margin);
			ScreenPosition.Y = FMath::Clamp(ScreenPosition.Y, margin, height - margin);

			SetPositionInViewport(ScreenPosition, true);
		}
	}
}
