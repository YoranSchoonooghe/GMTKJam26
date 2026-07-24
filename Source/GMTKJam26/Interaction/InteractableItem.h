#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableItem.generated.h"

class UStaticMeshComponent;
class UBlobShadowComponent;

UENUM(BlueprintType)
enum class EInteractableItemState : uint8
{
	Grounded,
	Held,
	Flying,
	Attached
};

class ACharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemLandedSignature, FVector, HitLocation);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemAttachedSignature, FVector, AttachLocation);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnItemFlightStartedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemHitCharacterSignature, ACharacter*, HitCharacter);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBumpedByCharacterSignature, ACharacter*, BumpingCharacter);

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

	void SnapToAttachPoint(const FVector& Location, const FRotator& Rotation);

	UFUNCTION(BlueprintPure, Category = "Interaction")
	EInteractableItemState GetItemState() const { return State; }

	UFUNCTION(BlueprintPure, Category = "Interaction")
	AActor* GetLastHolder() const { return LastHolder.Get(); }

	UFUNCTION(BlueprintPure, Category = "Interaction")
	UStaticMeshComponent* GetItemMesh() const { return Mesh; }

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void SetHighlight(bool highlighted);

	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnItemLandedSignature OnLanded;
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnItemAttachedSignature OnAttached;

	// Fires when this item is released via Throw() (covers the self-hit and any later throw) -
	// distinct from UThrowComponent::OnItemThrown, which is on the thrower's own component.
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnItemFlightStartedSignature OnThrown;

	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnItemHitCharacterSignature OnHitCharacter;

	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnBumpedByCharacterSignature OnBumpedByCharacter;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Blob Shadow", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBlobShadowComponent> BlobShadowComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction")
	TObjectPtr<UMaterialInterface> OutlineMaterial;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	float PickupCooldown = 0.2f;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction|Push")
	float ThrownHitPushForce = 500.f;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction|Push")
	float ThrownHitPushUpwardForce = 150.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	float AttachDuration{ 0.3f };

public:
	virtual void Tick(float DeltaTime) override;

private:
	UFUNCTION()
	void OnMeshHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void ResetPickupCooldown();

	void UpdateAttachedState(float DeltaTime);

	EInteractableItemState State = EInteractableItemState::Grounded;
	bool bCanBePickedUp = true;
	FTimerHandle PickupCooldownTimerHandle;

	TWeakObjectPtr<AActor> LastHolder;

	bool _bIsAttaching{ false };
	float _elapsedAttachTime{ 0.0f };

	FVector _startLocation{};
	FVector _targetLocation{};
	FRotator _startRotation{};
	FRotator _targetRotation{};
};
