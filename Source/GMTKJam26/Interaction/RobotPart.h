#pragma once

#include "CoreMinimal.h"
#include "Interaction/InteractableItem.h"
#include "RobotPart.generated.h"

UENUM(BlueprintType)
enum class EPartType : uint8
{
	Head,
	Torso,
	LeftArm,
	RightArm,
	LeftLeg,
	RightLeg
};

UCLASS()
class GMTKJAM26_API ARobotPart : public AInteractableItem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Robot Part")
	float GetSeconds() const { return Seconds; }

	UFUNCTION(BlueprintCallable, Category = "Robot Part")
	void SetSeconds(float NewSeconds) { Seconds = NewSeconds; }

	EPartType GetPartType() const { return PartType; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot Part", meta = (ClampMin = "0.0"))
	float Seconds = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot Part")
	EPartType PartType{ EPartType::Head };
};
