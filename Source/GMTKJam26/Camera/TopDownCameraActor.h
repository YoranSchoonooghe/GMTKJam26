#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TopDownCameraActor.generated.h"

class UCameraComponent;
class UCameraShakeComponent;

UCLASS()
class GMTKJAM26_API ATopDownCameraActor : public AActor
{
	GENERATED_BODY()

public:
	ATopDownCameraActor();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraShakeComponent> CameraShakeComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	TSubclassOf<UCameraShakeBase> DeathCameraShake;
	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	TSubclassOf<UCameraShakeBase> ItemPlacementCameraShake;

private:
	UFUNCTION()
	void RequestDeathCameraShake();
	UFUNCTION()
	void RequestItemPlacementCameraShake();
};
