#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TimerManager.h"
#include "GameModeGMTK.generated.h"

class UMenuStateBase;

UCLASS()
class GMTKJAM26_API AGameModeGMTK : public AGameModeBase
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	UPROPERTY(EditDefaultsOnly, Category = "Players")
	int32 NumberOfLocalPlayers = 2;

	UPROPERTY(EditDefaultsOnly, Category = "GameOver")
	TSubclassOf<UMenuStateBase> GameOverStateClass;

private:
	void BindPlayerTimerEvents();

	UFUNCTION()
	void HandlePlayer1TimerExpired();

	UFUNCTION()
	void HandlePlayer2TimerExpired();

	void HandleGameOver(int32 LosingPlayerIndex);

	int32 NextPlayerStartIndex = 0;
	bool bGameOverTriggered = false;

	FTimerHandle BindTimerEventsRetryHandle;
};
