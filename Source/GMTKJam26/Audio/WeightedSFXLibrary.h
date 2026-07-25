#pragma once

#include "CoreMinimal.h"
#include "WeightedSFX.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "WeightedSFXLibrary.generated.h"

UCLASS()
class GMTKJAM26_API UWeightedSFXLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, Category = "Audio")
	static USoundBase* GetRandomSound(const FWeightedSFXList& SoundsList);
};
