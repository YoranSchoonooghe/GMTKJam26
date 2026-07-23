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
}

void URespawnCountdownWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	Elapsed += InDeltaTime;

	if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
	{
		FVector2D ScreenPosition;
		if (PC->ProjectWorldLocationToScreen(WorldLocation, ScreenPosition))
		{
			SetPositionInViewport(ScreenPosition, true);
		}
	}

	if (CountdownLabel)
	{
		const float Remaining = FMath::Max(Duration - Elapsed, 0.f);
		CountdownLabel->SetText(FText::AsNumber(FMath::CeilToInt(Remaining)));
	}
}
