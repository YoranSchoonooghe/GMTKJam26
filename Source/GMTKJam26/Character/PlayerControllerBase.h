#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "PlayerControllerBase.generated.h"

class UInputMappingContext;
class UInputAction;
class APlayerCharacter;
class UPlayerTimerWidget;
class UMenuStateBase;

UCLASS()
class GMTKJAM26_API APlayerControllerBase : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UPlayerTimerWidget> TimerWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Menu")
	TSubclassOf<UMenuStateBase> DefaultRootMenuState;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> PauseAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> DashAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> InteractAction;

private:
	void Move(const FInputActionValue& Value);
	void StartJump();
	void StopJump();
	void StartDash();
	void StartInteract();
	void RequestPauseToggle();

	UPROPERTY()
	TObjectPtr<APlayerCharacter> ControlledCharacter;

	UPROPERTY()
	TObjectPtr<UPlayerTimerWidget> TimerWidget;
};
