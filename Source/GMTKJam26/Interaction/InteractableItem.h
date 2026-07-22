#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableItem.generated.h"

class UStaticMeshComponent;

UENUM(BlueprintType)
enum class EInteractableItemState : uint8
{
	Grounded,
	Held,
	Flying
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemLandedSignature, FVector, HitLocation);

UCLASS()
class GMTKJAM26_API AInteractableItem : public AActor
{
	GENERATED_BODY()

public:
	AInteractableItem();

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	bool CanBePickedUp() const;

	void OnPickedUp(USceneComponent* AttachTo, FName SocketName);

	void Release();

	void Throw(const FVector& Velocity);

	UFUNCTION(BlueprintPure, Category = "Interaction")
	EInteractableItemState GetItemState() const { return State; }

	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnItemLandedSignature OnLanded;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	float PickupCooldown = 0.2f;

private:
	UFUNCTION()
	void OnMeshHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void ResetPickupCooldown();

	EInteractableItemState State = EInteractableItemState::Grounded;
	bool bCanBePickedUp = true;
	FTimerHandle PickupCooldownTimerHandle;
};
