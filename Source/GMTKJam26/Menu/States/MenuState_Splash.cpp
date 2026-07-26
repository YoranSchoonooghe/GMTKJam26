#include "MenuState_Splash.h"
#include "../MenuFlowSubsystem.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"

void UMenuState_Splash::EnterState(UMenuFlowSubsystem* Flow)
{
	Super::EnterState(Flow);

	PendingFlow = Flow;

	UWorld* World = Flow && Flow->GetGameInstance() ? Flow->GetGameInstance()->GetWorld() : nullptr;
	if (World)
	{
		World->GetTimerManager().SetTimer(SplashTimerHandle, this, &UMenuState_Splash::FinishSplash, SplashDuration, false);
	}
}

void UMenuState_Splash::ExitState(UMenuFlowSubsystem* Flow)
{
	UWorld* World = Flow && Flow->GetGameInstance() ? Flow->GetGameInstance()->GetWorld() : nullptr;
	if (World)
	{
		World->GetTimerManager().ClearTimer(SplashTimerHandle);
	}

	Super::ExitState(Flow);
}

void UMenuState_Splash::FinishSplash()
{
	if (UMenuFlowSubsystem* Flow = PendingFlow.Get())
	{
		if (NextStateClass)
		{
			Flow->SetRootState(NextStateClass);
		}
	}
}
