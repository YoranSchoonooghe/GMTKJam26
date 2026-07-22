#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PickupComponent.generated.h"

class UBoxComponent;
class AInteractableItem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPickupActionSignature);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GMTKJAM26_API UPickupComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPickupComponent();

	UFUNCTION(BlueprintCallable, Category = "Pickup")
	void TryPickup();

	UFUNCTION(BlueprintCallable, Category = "Pickup")
	void DropItem();

	UFUNCTION(BlueprintPure, Category = "Pickup")
	bool IsHoldingItem() const { return HeldItem != nullptr; }

	UFUNCTION(BlueprintPure, Category = "Pickup")
	AInteractableItem* GetHeldItem() const { return HeldItem; }

	AInteractableItem* TakeHeldItem();

	UPROPERTY(BlueprintAssignable, Category = "Pickup")
	FOnPickupActionSignature OnItemPickedUp;

	UPROPERTY(BlueprintAssignable, Category = "Pickup")
	FOnPickupActionSignature OnItemDropped;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Pickup")
	FName HoldSocketName = FName("HoldSocket");

	UPROPERTY(EditDefaultsOnly, Category = "Pickup")
	FVector InteractionZoneExtent = FVector(75.f, 75.f, 50.f);

	UPROPERTY(EditDefaultsOnly, Category = "Pickup")
	FVector InteractionZoneOffset = FVector(100.f, 0.f, 0.f);

private:
	void EquipItem(AInteractableItem* ItemToEquip);

	UPROPERTY()
	TObjectPtr<UBoxComponent> InteractionZone;

	UPROPERTY()
	TObjectPtr<AInteractableItem> HeldItem;
};
