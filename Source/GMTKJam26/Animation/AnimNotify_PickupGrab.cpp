#include "AnimNotify_PickupGrab.h"
#include "Components/SkeletalMeshComponent.h"
#include "GMTKJam26/Character/PlayerCharacter.h"

void UAnimNotify_PickupGrab::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp)
	{
		return;
	}

	if (APlayerCharacter* Character = Cast<APlayerCharacter>(MeshComp->GetOwner()))
	{
		Character->ExecutePickupGrab();
	}
}
