#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BaseFocusWidget.generated.h"

class UButton;

UCLASS(Abstract)
class GMTKJAM26_API UBaseFocusWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Focus", meta = (BindWidgetOptional))
	TObjectPtr<UButton> FirstFocusedElement;

private:
	void FocusButton(UButton* Button) const;

	TWeakObjectPtr<UButton> LastFocusedButton;
};
