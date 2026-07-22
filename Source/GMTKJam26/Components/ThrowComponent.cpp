#include "ThrowComponent.h"
#include "PickupComponent.h"
#include "Interaction/InteractableItem.h"

UThrowComponent::UThrowComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UThrowComponent::BeginPlay()
{
	Super::BeginPlay();

	if (AActor* Owner = GetOwner())
	{
		PickupComponent = Owner->FindComponentByClass<UPickupComponent>();
	}
}

void UThrowComponent::ThrowHeldItem()
{
	if (!PickupComponent || !PickupComponent->IsHoldingItem())
	{
		return;
	}

	AInteractableItem* Item = PickupComponent->TakeHeldItem();
	if (!Item)
	{
		return;
	}

	FRotator ThrowRotation = GetOwner()->GetActorRotation();
	ThrowRotation.Pitch += ThrowAngle;

	const FVector ThrowVelocity = ThrowRotation.Vector() * ThrowForce;
	Item->Throw(ThrowVelocity);

	OnItemThrown.Broadcast();
}
