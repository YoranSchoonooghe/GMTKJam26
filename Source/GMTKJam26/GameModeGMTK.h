#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TimerManager.h"
#include "GameModeGMTK.generated.h"

class UMenuStateBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameOverSequenceStartedSignature, APawn*, LosingPawn);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameOverSequenceEndedSignature, int32, WinningPlayerIndex);

UCLASS()
class GMTKJAM26_API AGameModeGMTK : public AGameModeBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "GameOver")
	FOnGameOverSequenceStartedSignature OnGameOverSequenceStarted;

	UPROPERTY(BlueprintAssignable, Category = "GameOver")
	FOnGameOverSequenceEndedSignature OnGameOverSequenceEnded;

protected:
	virtual void BeginPlay() override;

	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	UPROPERTY(EditDefaultsOnly, Category = "Players")
	int32 NumberOfLocalPlayers = 2;

	UPROPERTY(EditDefaultsOnly, Category = "GameOver")
	TSubclassOf<UMenuStateBase> GameOverStateClass;

	UPROPERTY(EditDefaultsOnly, Category = "GameOver", meta = (ClampMin = "0.0"))
	float GameOverDelaySeconds = 2.f;

private:
	void BindPlayerTimerEvents();
	void BindPlayerRespawnEvents();

	UFUNCTION()
	void HandlePlayer1TimerExpired();

	UFUNCTION()
	void HandlePlayer2TimerExpired();

	void HandleGameOver(int32 LosingPlayerIndex);
	void ShowGameOverMenu();

	int32 NextPlayerStartIndex = 0;
	int32 PendingWinningPlayerIndex = -1;
	bool bGameOverTriggered = false;

	FTimerHandle BindTimerEventsRetryHandle;
	FTimerHandle BindRespawnEventsRetryHandle;
	FTimerHandle GameOverDelayTimerHandle;
};
