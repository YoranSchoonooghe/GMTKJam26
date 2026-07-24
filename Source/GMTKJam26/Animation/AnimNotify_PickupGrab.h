#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_PickupGrab.generated.h"

UCLASS(meta = (DisplayName = "Pickup Grab"))
class GMTKJAM26_API UAnimNotify_PickupGrab : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

#if WITH_EDITOR
	virtual FString GetNotifyName_Implementation() const override { return TEXT("Pickup Grab"); }
#endif
};
