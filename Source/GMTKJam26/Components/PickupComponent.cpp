#include "PickupComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "Interaction/InteractableItem.h"

UPickupComponent::UPickupComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPickupComponent::BeginPlay()
{
	Super::BeginPlay();

	if (AActor* Owner = GetOwner())
	{
		InteractionZone = NewObject<UBoxComponent>(Owner, TEXT("PickupInteractionZone"));
		InteractionZone->SetBoxExtent(InteractionZoneExtent);
		InteractionZone->SetRelativeLocation(InteractionZoneOffset);
		InteractionZone->SetCollisionProfileName(FName("OverlapAllDynamic"));
		InteractionZone->RegisterComponent();
		InteractionZone->AttachToComponent(Owner->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	}
}

void UPickupComponent::TryPickup()
{
	if (HeldItem || !InteractionZone)
	{
		return;
	}

	TArray<AActor*> OverlappingActors;
	InteractionZone->GetOverlappingActors(OverlappingActors, AInteractableItem::StaticClass());

	AInteractableItem* ClosestItem = nullptr;
	float ClosestDistSq = TNumericLimits<float>::Max();
	const FVector OwnerLocation = GetOwner()->GetActorLocation();

	for (AActor* Actor : OverlappingActors)
	{
		AInteractableItem* Item = Cast<AInteractableItem>(Actor);
		if (!Item || !Item->CanBePickedUp())
		{
			continue;
		}

		const float DistSq = FVector::DistSquared(OwnerLocation, Item->GetActorLocation());
		if (DistSq < ClosestDistSq)
		{
			ClosestDistSq = DistSq;
			ClosestItem = Item;
		}
	}

	if (ClosestItem)
	{
		EquipItem(ClosestItem);
	}
}

void UPickupComponent::EquipItem(AInteractableItem* ItemToEquip)
{
	ACharacter* OwningCharacter = Cast<ACharacter>(GetOwner());
	if (!ItemToEquip || !OwningCharacter || !OwningCharacter->GetMesh())
	{
		return;
	}

	HeldItem = ItemToEquip;
	HeldItem->OnPickedUp(OwningCharacter->GetMesh(), HoldSocketName);

	OnItemPickedUp.Broadcast();
}

void UPickupComponent::DropItem()
{
	if (!HeldItem)
	{
		return;
	}

	HeldItem->Release();
	HeldItem = nullptr;

	OnItemDropped.Broadcast();
}

AInteractableItem* UPickupComponent::TakeHeldItem()
{
	AInteractableItem* Item = HeldItem;
	HeldItem = nullptr;
	return Item;
}
