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
	bool TryPickup();

	UFUNCTION(BlueprintCallable, Category = "Pickup")
	void DropItem();

	UFUNCTION(BlueprintPure, Category = "Pickup")
	bool IsHoldingItem() const { return HeldItem != nullptr; }

	UFUNCTION(BlueprintPure, Category = "Pickup")
	AInteractableItem* GetHeldItem() const { return HeldItem; }

	UFUNCTION(BlueprintPure, Category = "Pickup")
	bool HasNearbyItem() const { return HighlightedItem != nullptr; }

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

	UPROPERTY(EditDefaultsOnly, Category = "Pickup")
	float PickupCooldown = 0.5f;

	UFUNCTION()
	void OnBeginOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 BodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 BodyIndex);

	UPROPERTY()
	TArray<AInteractableItem*> NearbyItems;

	UPROPERTY()
	TObjectPtr<AInteractableItem> HighlightedItem;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	void EquipItem(AInteractableItem* ItemToEquip);

	UPROPERTY()
	TObjectPtr<UBoxComponent> InteractionZone;

	UPROPERTY()
	TObjectPtr<AInteractableItem> HeldItem;

	FTimerHandle PickupCooldownTimerHandle;
};
