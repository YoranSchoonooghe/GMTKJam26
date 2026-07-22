#pragma once

#include "CoreMinimal.h"
#include "Interaction/InteractableItem.h"
#include "RobotPart.generated.h"

UCLASS()
class GMTKJAM26_API ARobotPart : public AInteractableItem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Robot Part")
	float GetSeconds() const { return Seconds; }

	UFUNCTION(BlueprintCallable, Category = "Robot Part")
	void SetSeconds(float NewSeconds) { Seconds = NewSeconds; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot Part", meta = (ClampMin = "0.0"))
	float Seconds = 1.f;
};
