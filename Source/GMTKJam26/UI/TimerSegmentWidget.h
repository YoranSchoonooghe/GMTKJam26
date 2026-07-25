#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/PlayerTimerComponent.h"
#include "TimerSegmentWidget.generated.h"

class UProgressBar;
class UTextBlock;
class UTexture2D;


UCLASS()
class GMTKJAM26_API UTimerSegmentWidget : public UUserWidget
{
	GENERATED_BODY()

public:
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
