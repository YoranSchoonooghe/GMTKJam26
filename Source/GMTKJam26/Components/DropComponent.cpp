#include "DropComponent.h"
#include "PickupComponent.h"
#include "PushComponent.h"
#include "RespawnComponent.h"
#include "Interaction/InteractableItem.h"

UDropComponent::UDropComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}


void UDropComponent::BeginPlay()
{
	Super::BeginPlay();

	if (AActor* Owner = GetOwner())
	{
		PickupComponent = Owner->FindComponentByClass<UPickupComponent>();
		
		auto PushComponent = Owner->FindComponentByClass<UPushComponent>();
		if (PushComponent)
		{
			PushComponent->OnKnockback.AddDynamic(this, &UDropComponent::HandleKnockBack);
		}

		auto RespawnComponent = Owner->FindComponentByClass<URespawnComponent>();
		if (RespawnComponent)
		{
			RespawnComponent->OnPlayerDied.AddDynamic(this, &UDropComponent::HandlePlayerDied);
		}
	}
}

void UDropComponent::DropHeldItem()
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

	const FVector launchDirection = FMath::VRandCone(FVector::UpVector, FMath::DegreesToRadians(45.0f));
	const float launchPower = FMath::FRandRange(LaunchPower - LaunchPowerDeviation, LaunchPower + LaunchPowerDeviation);

	Item->Throw(launchDirection * launchPower);

	OnItemLost.Broadcast();
}


void UDropComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UDropComponent::HandleKnockBack(FVector)
{
	DropHeldItem();
}

void UDropComponent::HandlePlayerDied(FVector)
{
	DropHeldItem();
}

