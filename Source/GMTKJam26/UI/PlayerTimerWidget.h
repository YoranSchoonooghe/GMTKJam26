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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timer")
	bool bMirrorIcon = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timer")
	FVector2D Offset = FVector2D::ZeroVector;
};


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

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UCanvasPanel> SegmentContainer;

	UPROPERTY(EditDefaultsOnly, Category = "Timer")
	TSubclassOf<UTimerSegmentWidget> SegmentWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Timer")
	TArray<FTimerSegmentVisual> ItemVisuals;

	UPROPERTY(EditDefaultsOnly, Category = "Timer")
	FTimerSegmentVisual CoreVisual;

	UPROPERTY(EditDefaultsOnly, Category = "Timer")
	float GlobalScale = 0.05f;

	UPROPERTY(EditDefaultsOnly, Category = "Timer")
	float OverlapAmount = 20.f;

private:
	void BuildSegmentWidgets();

	UPROPERTY()
	TObjectPtr<UPlayerTimerComponent> TimerComponent;

	UPROPERTY()
	TArray<TObjectPtr<UTimerSegmentWidget>> SegmentWidgets;
};
