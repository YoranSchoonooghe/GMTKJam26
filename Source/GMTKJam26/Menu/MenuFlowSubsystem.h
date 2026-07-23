#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MenuFlowSubsystem.generated.h"

class UMenuStateBase;
class APlayerController;

UCLASS()
class GMTKJAM26_API UMenuFlowSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Menu Flow")
	void SetRootState(TSubclassOf<UMenuStateBase> StateClass);

	UFUNCTION(BlueprintCallable, Category = "Menu Flow")
	void PushState(TSubclassOf<UMenuStateBase> StateClass);

	UFUNCTION(BlueprintCallable, Category = "Menu Flow")
	void PopState();

	UFUNCTION(BlueprintCallable, Category = "Menu Flow")
	void RequestBack();

	UFUNCTION(BlueprintPure, Category = "Menu Flow")
	UMenuStateBase* GetCurrentState() const;

	APlayerController* GetFirstPlayerController() const;

	UFUNCTION(BlueprintCallable, Category = "Menu Flow")
	void RestartLevel();

	UFUNCTION(BlueprintCallable, Category = "Menu Flow")
	void SetGameOverWinnerIndex(int32 InWinningPlayerIndex) { WinningPlayerIndex = InWinningPlayerIndex; }

	UFUNCTION(BlueprintPure, Category = "Menu Flow")
	int32 GetGameOverWinnerIndex() const { return WinningPlayerIndex; }

private:
	UPROPERTY()
	TArray<TObjectPtr<UMenuStateBase>> StateStack;

	int32 WinningPlayerIndex = -1;
};
