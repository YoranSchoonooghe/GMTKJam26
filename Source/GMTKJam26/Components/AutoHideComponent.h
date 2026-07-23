#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TimerManager.h"
#include "AutoHideComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GMTKJAM26_API UAutoHideComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UAutoHideComponent();

	UFUNCTION(BlueprintCallable, Category = "AutoHide")
	void Show();

	UFUNCTION(BlueprintCallable, Category = "AutoHide")
	void Hide();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "AutoHide")
	float HideDelay = 4.f;

	UPROPERTY(EditDefaultsOnly, Category = "AutoHide")
	bool bToggleCollision = true;

private:
	void BindToPlayerRespawnEvents();

	UFUNCTION()
	void OnAnyPlayerDied(FVector DeathLocation);

	TWeakObjectPtr<AActor> OwningActor;

	FTimerHandle HideTimerHandle;
	FTimerHandle BindRetryTimerHandle;
};
