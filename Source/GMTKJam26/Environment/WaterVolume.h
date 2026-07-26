#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WaterVolume.generated.h"

class UBoxComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnWaterSplashSignature, AActor*, SplashedActor, FVector, SplashLocation, float, ImpactSpeed);

UCLASS()
class GMTKJAM26_API AWaterVolume : public AActor
{
	GENERATED_BODY()

public:
	AWaterVolume();

	UPROPERTY(BlueprintAssignable, Category = "Water")
	FOnWaterSplashSignature OnWaterSplash;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Water", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> WaterZone;

	UPROPERTY(EditAnywhere, Category = "Water")
	float MinDownwardSpeed = 50.f;

private:
	UFUNCTION()
	void OnZoneOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
