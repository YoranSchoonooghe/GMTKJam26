#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/PlayerTimerComponent.h"
#include "TimerSegmentWidget.generated.h"

class UProgressBar;
class UTextBlock;
class UTexture2D;
class UWidgetAnimation;


UCLASS()
class GMTKJAM26_API UTimerSegmentWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Timer")
	void SetVisual(const FText& InDisplayName, UTexture2D* InFillIcon, UTexture2D* InBackgroundIcon, const FVector2D& InOffset);

	UFUNCTION(BlueprintCallable, Category = "Timer")
	void UpdateSegment(const FTimerSegment& Segment, bool bIsActive, float DeltaTime);

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

	UPROPERTY(Transient, BlueprintReadOnly, meta = (BindWidgetAnimOptional))
	TObjectPtr<UWidgetAnimation> PopInAnim;

	UPROPERTY(Transient, BlueprintReadOnly, meta = (BindWidgetAnimOptional))
	TObjectPtr<UWidgetAnimation> PopLostAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timer")
	float LostTimeJumpThreshold = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timer")
	float LowTimeShakeThreshold = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timer")
	float MaxLowTimeShakeAngle = 2.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timer")
	float MinLowTimeShakeFrequency = 14.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timer")
	float MaxLowTimeShakeFrequency = 35.f;

private:
	void ApplyVisual();

	bool bWasActive = false;
	bool bWasShaking = false;
	bool bHasPreviousRemainingTime = false;
	float PreviousRemainingTime = 0.f;
	float ShakePhase = 0.f;
};
