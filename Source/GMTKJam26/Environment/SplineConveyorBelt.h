#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ConveyorBelt.h"
#include "SplineConveyorBelt.generated.h"

class USplineComponent;
class USplineMeshComponent;
class UBoxComponent;
class UStaticMesh;
class AInteractableItem;

UCLASS()
class GMTKJAM26_API ASplineConveyorBelt : public AActor
{
	GENERATED_BODY()

public:
	ASplineConveyorBelt();

	virtual void Tick(float DeltaTime) override;
	virtual void OnConstruction(const FTransform& Transform) override;

	UFUNCTION(BlueprintCallable, Category = "Conveyor|Spawning")
	void EnableSpawning();

	UFUNCTION(BlueprintCallable, Category = "Conveyor|Spawning")
	void DisableSpawning();

	UPROPERTY(BlueprintAssignable, Category = "Conveyor|Events")
	FOnItemSpawnedSignature OnItemSpawned;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Conveyor", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USplineComponent> ConveyorSpline;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Conveyor", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> CarryZone;

	UPROPERTY(EditAnywhere, Category = "Conveyor|Visuals")
	TObjectPtr<UStaticMesh> BeltSegmentMesh;

	UPROPERTY(EditAnywhere, Category = "Conveyor|Visuals", meta = (ClampMin = "10.0"))
	float SegmentLength = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conveyor")
	float ConveyorSpeed = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conveyor")
	float ConveyorAcceleration = 2000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conveyor")
	float AngularDampingRate = 3000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conveyor", meta = (ClampMin = "0.0", ClampMax = "90.0"))
	float UphillGravityDisableThresholdDegrees = 15.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conveyor")
	float MaxRideDistance = 100.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Conveyor|Spawning", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> SpawnPoint;

	UPROPERTY(EditAnywhere, Category = "Conveyor|Spawning")
	TArray<FConveyorSpawnEntry> ItemsToSpawn;

	UPROPERTY(EditAnywhere, Category = "Conveyor|Spawning", meta = (ClampMin = "0.0"))
	float MinSpawnInterval = 2.f;

	UPROPERTY(EditAnywhere, Category = "Conveyor|Spawning", meta = (ClampMin = "0.0"))
	float MaxSpawnInterval = 5.f;

private:
	void SpawnItem();
	void ScheduleNextSpawn();
	TSubclassOf<AInteractableItem> PickRandomItemClass() const;

	void RebuildCarryZone();
	void RebuildVisualSegments();

	UPROPERTY()
	TArray<TObjectPtr<USplineMeshComponent>> VisualSegments;

	TMap<TWeakObjectPtr<AActor>, float> RidingProgress;

	FTimerHandle SpawnTimerHandle;
	bool bSpawningEnabled = true;
};
