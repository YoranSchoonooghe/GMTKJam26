#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerTimerWidget.generated.h"

class UCanvasPanel;
class UPlayerTimerComponent;
class UTimerSegmentWidget;
class UTexture2D;

USTRUCT(BlueprintType)
struct FTimerSegmentVisual
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timer")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timer")
	TObjectPtr<UTexture2D> FillIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timer")
	TObjectPtr<UTexture2D> BackgroundIcon;

	// Flips the icon horizontally (Render Scale X = -1) - lets a "right" icon double as its
	// mirrored "left" counterpart instead of needing a separate mirrored texture.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timer")
	bool bMirrorIcon = false;

	// Fine-tune nudge applied on top of this segment's auto-computed position, for closing up any
	// residual seam against its neighbor without touching the automatic layout math.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timer")
	FVector2D Offset = FVector2D::ZeroVector;
};

// Spawns and lays out one UTimerSegmentWidget per configured segment (ItemVisuals in order, then
// CoreVisual last) side by side in SegmentContainer, sized directly from each icon's real texture
// dimensions and positioned to overlap by OverlapAmount so their borders interlock. All positioning
// is explicit pixel math on the Canvas Panel slots - no SizeBox/ScaleBox/auto-layout involved. The
// layout is built in NativePreConstruct, so it previews live in the Designer as you edit the data,
// not just at runtime.
UCLASS()
class GMTKJAM26_API UPlayerTimerWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Timer")
	void InitTimerComponent(UPlayerTimerComponent* InTimerComponent);

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	// Must be a Canvas Panel - segments are laid out here with explicit computed pixel rects.
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UCanvasPanel> SegmentContainer;

	UPROPERTY(EditDefaultsOnly, Category = "Timer")
	TSubclassOf<UTimerSegmentWidget> SegmentWidgetClass;

	// One entry per item segment, in the same order as the timer component's ItemOrder.
	UPROPERTY(EditDefaultsOnly, Category = "Timer")
	TArray<FTimerSegmentVisual> ItemVisuals;

	UPROPERTY(EditDefaultsOnly, Category = "Timer")
	FTimerSegmentVisual CoreVisual;

	// Multiplies each segment's native icon texture dimensions to get its on-screen size.
	UPROPERTY(EditDefaultsOnly, Category = "Timer")
	float GlobalScale = 0.05f;

	// How many (post-scale) pixels each segment overlaps into the previous one, so the baked-in
	// borders/diagonal cuts interlock instead of doubling up into a visible seam.
	UPROPERTY(EditDefaultsOnly, Category = "Timer")
	float OverlapAmount = 20.f;

private:
	void BuildSegmentWidgets();

	UPROPERTY()
	TObjectPtr<UPlayerTimerComponent> TimerComponent;

	UPROPERTY()
	TArray<TObjectPtr<UTimerSegmentWidget>> SegmentWidgets;
};
