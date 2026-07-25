#include "Audio/WeightedSFXLibrary.h"

USoundBase* UWeightedSFXLibrary::GetRandomSound(const FWeightedSFXList& SoundsList)
{
    check(!SoundsList.Sounds.IsEmpty());

    if (SoundsList.Sounds.Num() == 1)
    {
        return SoundsList.Sounds[0].Sound;
    }

    float TotalWeight = 0.0f;
    for (const FWeightedSFX& WeightedSound : SoundsList.Sounds)
    {
        TotalWeight += WeightedSound.Weight;
    }

    const float RandomWeight = FMath::FRandRange(0.0f, TotalWeight);

    float AccumulatedWeight = 0.0f;
    for (const FWeightedSFX& WeightedSound : SoundsList.Sounds)
    {
        AccumulatedWeight += WeightedSound.Weight;
        if (AccumulatedWeight >= RandomWeight)
        {
            return WeightedSound.Sound;
        }
    }

    return nullptr;
}