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
// outline heart underneath) rather than one icon tinted at runtime. Derive a Widget Blueprint from
// this class and bind TimeBar (required) / NameLabel / TimeLabel (optional) to design the look.
UCLASS()
class GMTKJAM26_API UTimerSegmentWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Timer")
	void SetDisplayName(const FText& InDisplayName);

	UFUNCTION(BlueprintCallable, Category = "Timer")
	void SetIcons(UTexture2D* FillIcon, UTexture2D* BackgroundIcon);

	UFUNCTION(BlueprintCallable, Category = "Timer")
	void UpdateSegment(const FTimerSegment& Segment, bool bIsActive);

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UProgressBar> TimeBar;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> NameLabel;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TimeLabel;
};
