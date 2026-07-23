#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RespawnCountdownWidget.generated.h"

class UTextBlock;

UCLASS()
class GMTKJAM26_API URespawnCountdownWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable, Category = "Respawn")
	void StartCountdown(const FVector& InWorldLocation, float InDuration);

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> CountdownLabel;

private:
	FVector WorldLocation = FVector::ZeroVector;
	float Duration = 5.f;
	float Elapsed = 0.f;
};
