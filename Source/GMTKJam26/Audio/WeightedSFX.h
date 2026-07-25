#pragma once

#include <cassert>

#include "CoreMinimal.h"
#include "Sound/SoundBase.h"
#include "WeightedSFX.generated.h"

USTRUCT(BlueprintType)
struct FWeightedSFX
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USoundBase> Sound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Weight = 1.0f;
};

USTRUCT(BlueprintType)
struct FWeightedSFXList
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FWeightedSFX> Sounds;
};

inline USoundBase* GetRandomSound(const FWeightedSFXList& soundsList)
{
	check(!soundsList.Sounds.IsEmpty());

	if (soundsList.Sounds.Num() == 1)
		return soundsList.Sounds[0].Sound;

	float totalWeight{ 0.0f };
	for (auto weightedSound : soundsList.Sounds)
	{
		totalWeight += weightedSound.Weight;
	}

	auto const randomWeight = FMath::FRandRange(0.0f, totalWeight);
	float accumulatedWeight{ 0.0f };
	for (auto weightedSound : soundsList.Sounds)
	{
		accumulatedWeight += weightedSound.Weight;
		if (accumulatedWeight >= randomWeight)
		{
			return weightedSound.Sound;
		}
	}

	return nullptr;
}