#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/PlayerTimerComponent.h"
#include "TimerSegmentWidget.generated.h"

class UProgressBar;
class UTextBlock;
class UTexture2D;

// Visualizes a single timer segment (one RobotPart slot, or the Core) as a ProgressBar whose Fill
// and Background are two separate icon textures (e.g. a colored heart wiping away to reveal a gray
// outline heart underneath). Place instances of this widget by hand in the Canvas Panel, then set
// DisplayName/FillIcon/BackgroundIcon/Offset per-instance in the Details panel - they preview
// immediately via NativePreConstruct, no compile or Play needed.
UCLASS()
class GMTKJAM26_API UTimerSegmentWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// Assigns and immediately applies this segment's display data - used when spawned dynamically.
	UFUNCTION(BlueprintCallable, Category = "Timer")
	void SetVisual(const FText& InDisplayName, UTexture2D* InFillIcon, UTexture2D* InBackgroundIcon, const FVector2D& InOffset);

	UFUNCTION(BlueprintCallable, Category = "Timer")
	void UpdateSegment(const FTimerSegment& Segment, bool bIsActive);

protected:
	virtual void NativePreConstruct() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timer")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timer")
	TObjectPtr<UTexture2D> FillIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timer")
	TObjectPtr<UTexture2D> BackgroundIcon;

	// Nudges this segment's on-screen position without touching its Canvas Slot position - handy
	// for fine-tuning interlock against its neighbor without recalculating absolute coordinates.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timer")
	FVector2D Offset = FVector2D::ZeroVector;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UProgressBar> TimeBar;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> NameLabel;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TimeLabel;

private:
	void ApplyVisual();
};
