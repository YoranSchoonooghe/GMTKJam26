#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TimerManager.h"
#include "RespawnComponent.generated.h"

class ACharacter;
class URespawnCountdownWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerDiedSignature, FVector, DeathLocation);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerRespawnedSignature);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GMTKJAM26_API URespawnComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	URespawnComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Respawn")
	void HandleFallIntoVoid();

	UPROPERTY(BlueprintAssignable, Category = "Respawn|Events")
	FOnPlayerDiedSignature OnPlayerDied;

	UPROPERTY(BlueprintAssignable, Category = "Respawn|Events")
	FOnPlayerRespawnedSignature OnPlayerRespawned;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Respawn")
	float VoidZThreshold = -500.f;

	UPROPERTY(EditDefaultsOnly, Category = "Respawn")
	float RespawnDelay = 5.f;

	UPROPERTY(EditDefaultsOnly, Category = "Respawn")
	TSubclassOf<URespawnCountdownWidget> CountdownWidgetClass;

private:
	void Respawn();

	TWeakObjectPtr<ACharacter> OwningCharacter;

	FVector SpawnLocation = FVector::ZeroVector;
	FRotator SpawnRotation = FRotator::ZeroRotator;

	bool bIsDead = false;

	UPROPERTY()
	TObjectPtr<URespawnCountdownWidget> CountdownWidget;

	FTimerHandle RespawnTimerHandle;
};
