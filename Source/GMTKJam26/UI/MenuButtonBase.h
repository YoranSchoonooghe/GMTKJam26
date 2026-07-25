#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "MenuButtonBase.generated.h"

UCLASS()
class GMTKJAM26_API UMenuButtonBase : public UButton
{
	GENERATED_BODY()

public:
	UMenuButtonBase(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu Button|Bump")
	float BumpScale = 1.08f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu Button|Bump")
	float BumpAngle = -3.f;

private:
	UFUNCTION()
	void HandleHovered();

	UFUNCTION()
	void HandleUnhovered();

	void HandleReceivedFocus();
	void HandleLostFocus();

	void RefreshBumpState();

	bool bIsMouseHovered = false;
	bool bIsFocused = false;
};
