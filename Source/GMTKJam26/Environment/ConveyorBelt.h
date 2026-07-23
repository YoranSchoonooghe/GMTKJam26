#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ConveyorBelt.generated.h"

class UStaticMeshComponent;
class UBoxComponent;
class AInteractableItem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemSpawnedSignature, AInteractableItem*, SpawnedItem);

USTRUCT(BlueprintType)
struct FConveyorSpawnEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conveyor|Spawning")
	TSubclassOf<AInteractableItem> ItemClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conveyor|Spawning", meta = (ClampMin = "0.0"))
	float SpawnWeight = 1.f;
};

UCLASS()
class GMTKJAM26_API AConveyorBelt : public AActor
{
	GENERATED_BODY()

public:
	AConveyorBelt();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Conveyor|Spawning")
	void EnableSpawning();

	UFUNCTION(BlueprintCallable, Category = "Conveyor|Spawning")
	void DisableSpawning();

	UPROPERTY(BlueprintAssignable, Category = "Conveyor|Events")
	FOnItemSpawnedSignature OnItemSpawned;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Conveyor", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> BeltMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Conveyor", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> CarryZone;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Conveyor|Spawning", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> SpawnPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conveyor")
	float ConveyorSpeed = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conveyor")
	float ConveyorAcceleration = 2000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conveyor|Spawning")
	TArray<FConveyorSpawnEntry> ItemsToSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conveyor|Spawning", meta = (ClampMin = "0.0"))
	float MinSpawnInterval = 2.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conveyor|Spawning", meta = (ClampMin = "0.0"))
	float MaxSpawnInterval = 5.f;

private:
	void SpawnItem();
	void ScheduleNextSpawn();
	TSubclassOf<AInteractableItem> PickRandomItemClass() const;

	FTimerHandle SpawnTimerHandle;
	bool bSpawningEnabled = true;
};
