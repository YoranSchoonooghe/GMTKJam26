#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemDespawnVolume.generated.h"

class UBoxComponent;

UCLASS()
class GMTKJAM26_API AItemDespawnVolume : public AActor
{
	GENERATED_BODY()

public:
	AItemDespawnVolume();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Despawn", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> DespawnZone;

private:
	UFUNCTION()
	void OnZoneOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
