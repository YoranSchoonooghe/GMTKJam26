#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemPlacementActor.generated.h"

class UBoxComponent;
class UWidgetComponent;
class AInteractableItem;

UCLASS()
class GMTKJAM26_API AItemPlacementActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AItemPlacementActor();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<UBoxComponent> TriggerBox;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<UStaticMeshComponent> PreviewMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UWidgetComponent> InteractWidget;

	UFUNCTION()
	void OnBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

public:	
	virtual void Tick(float DeltaTime) override;

private:
	UFUNCTION()
	void ShowAttachHint();
	UFUNCTION()
	void HideAttachHint();
	UFUNCTION()
	void TryAttach();

	bool _bHasItem{ false };
	bool _bPlayerIsInRange{ false };
	AInteractableItem* _targetItem{ nullptr };
};
