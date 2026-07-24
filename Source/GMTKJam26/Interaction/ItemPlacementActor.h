#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RobotPart.h"
#include "ItemPlacementActor.generated.h"

class UBoxComponent;
class UWidgetComponent;
class APlayerCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemPlacedSignature, ARobotPart*, PlacedItem);

UCLASS()
class GMTKJAM26_API AItemPlacementActor : public AActor
{
	GENERATED_BODY()

public:
	AItemPlacementActor();

	virtual void OnConstruction(const FTransform& Transform) override;

	UPROPERTY(BlueprintAssignable, Category = "Placement")
	FOnItemPlacedSignature OnItemPlaced;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<UBoxComponent> TriggerBox;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<UStaticMeshComponent> PreviewMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UWidgetComponent> InteractWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot Part")
	TSubclassOf<ARobotPart> ExpectedItemClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Robot Part")
	EPartType ExpectedPartType{ EPartType::Head };

	UPROPERTY(EditAnywhere, Category = "Robot Part")
	TObjectPtr<UMaterialInterface> GhostMaterial;

	UPROPERTY(EditAnywhere, Category = "Robot Part")
	FName GhostOpacityParameterName = "Opacity";

	UPROPERTY(EditAnywhere, Category = "Robot Part", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float GhostOpacity = 0.35f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target Player")
	int32 PlayerIndex{ 0 };

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
	void RetrievePlayerItem();
	UFUNCTION()
	void TryAttach();
	UFUNCTION()
	void ClearTargetItem();

	void RefreshPreviewFromExpectedItem();

	bool _bHasItem{ false };
	bool _bPlayerIsInRange{ false };
	ARobotPart* _targetItem{ nullptr };
	APlayerCharacter* _targetPlayer{ nullptr };
};
