#include "PickupComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "Interaction/InteractableItem.h"

UPickupComponent::UPickupComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
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
		InteractionZone->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Ignore);
		InteractionZone->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);
		InteractionZone->RegisterComponent();
		InteractionZone->AttachToComponent(Owner->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);

		InteractionZone->OnComponentBeginOverlap.AddDynamic(this, &UPickupComponent::OnBeginOverlap);
		InteractionZone->OnComponentEndOverlap.AddDynamic(this, &UPickupComponent::OnEndOverlap);
	}
}

bool UPickupComponent::TryPickup()
{
	if (HeldItem || !InteractionZone || GetWorld()->GetTimerManager().IsTimerActive(PickupCooldownTimerHandle))
	{
		return false;
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

		if (HighlightedItem)
		{
			HighlightedItem->SetHighlight(false);
		}

		GetWorld()->GetTimerManager().SetTimer(PickupCooldownTimerHandle, PickupCooldown, false);

		return true;
	}

	return false;
}

void UPickupComponent::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AInteractableItem* Item = Cast<AInteractableItem>(OtherActor);
	if (!Item || !Item->CanBePickedUp()) return;
	
	NearbyItems.AddUnique(Item);
}

void UPickupComponent::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (AInteractableItem* Item = Cast<AInteractableItem>(OtherActor))
	{
		NearbyItems.Remove(Item);
	}
}

void UPickupComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (HeldItem) return;

	AInteractableItem* closestItem = nullptr;
	float smallestDistance = MAX_FLT;

	for (AInteractableItem* Item : NearbyItems)
	{
		if (!IsValid(Item) || !Item->CanBePickedUp())
		{
			continue;
		}

		float DistSq = FVector::DistSquared(GetOwner()->GetActorLocation(), Item->GetActorLocation());

		if (DistSq < smallestDistance)
		{
			smallestDistance = DistSq;
			closestItem = Item;
		}
	}

	if (HighlightedItem != closestItem)
	{
		if (HighlightedItem)
		{
			HighlightedItem->SetHighlight(false);
		}

		HighlightedItem = closestItem;

		if (HighlightedItem)
		{
			HighlightedItem->SetHighlight(true);
		}
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
