#pragma once

#include "CoreMinimal.h"
#include "MenuStateBase.h"
#include "TimerManager.h"
#include "MenuState_Splash.generated.h"

UCLASS()
class GMTKJAM26_API UMenuState_Splash : public UMenuStateBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Menu")
	TSubclassOf<UMenuStateBase> NextStateClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Menu", meta = (ClampMin = "0.0"))
	float SplashDuration = 3.f;

	virtual void EnterState(UMenuFlowSubsystem* Flow) override;
	virtual void ExitState(UMenuFlowSubsystem* Flow) override;

private:
	void FinishSplash();

	TWeakObjectPtr<UMenuFlowSubsystem> PendingFlow;
	FTimerHandle SplashTimerHandle;
};
