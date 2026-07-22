#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ConveyorBelt.generated.h"

class UStaticMeshComponent;
class UBoxComponent;
class AInteractableItem;

UCLASS()
class GMTKJAM26_API AConveyorBelt : public AActor
{
	GENERATED_BODY()

public:
	AConveyorBelt();

	virtual void Tick(float DeltaTime) override;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conveyor|Spawning")
	TSubclassOf<AInteractableItem> ItemClassToSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conveyor|Spawning")
	float SpawnInterval = 3.f;

private:
	void SpawnItem();

	FTimerHandle SpawnTimerHandle;
};
