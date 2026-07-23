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
class UForceFeedbackEffect;

UCLASS()
class GMTKJAM26_API APlayerControllerBase : public APlayerController
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Rumble")
	void PlayRumble(float Intensity = 0.5f, float Duration = 0.2f, bool bAffectsLeftLarge = true, bool bAffectsLeftSmall = true, bool bAffectsRightLarge = true, bool bAffectsRightSmall = true);

	UFUNCTION(BlueprintCallable, Category = "Rumble")
	void PlayRumbleEffect(UForceFeedbackEffect* Effect, bool bLooping = false, FName Tag = NAME_None);

	UFUNCTION(BlueprintCallable, Category = "Rumble")
	void StopRumbleEffect(FName Tag);

	UFUNCTION(BlueprintCallable, Category = "Rumble")
	void NotifyItemBumped();

	UFUNCTION(BlueprintCallable, Category = "Rumble")
	void NotifyOpponentDied(FVector DeathLocation);

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

	UPROPERTY(EditDefaultsOnly, Category = "Rumble")
	TObjectPtr<UForceFeedbackEffect> KnockbackRumbleEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Rumble")
	TObjectPtr<UForceFeedbackEffect> DashHitRumbleEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Rumble")
	TObjectPtr<UForceFeedbackEffect> SegmentDepletedRumbleEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Rumble")
	TObjectPtr<UForceFeedbackEffect> PickupRumbleEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Rumble")
	TObjectPtr<UForceFeedbackEffect> ThrowRumbleEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Rumble")
	TObjectPtr<UForceFeedbackEffect> ItemBumpRumbleEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Rumble")
	TObjectPtr<UForceFeedbackEffect> DeathRumbleEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Rumble")
	TObjectPtr<UForceFeedbackEffect> KillRumbleEffect;

private:
	void Move(const FInputActionValue& Value);
	void StartJump();
	void StopJump();
	void StartDash();
	void StartInteract();
	void RequestPauseToggle();

	UFUNCTION()
	void HandleKnockbackRumble(FVector SourceLocation);

	UFUNCTION()
	void HandleDashHitRumble(class ACharacter* HitCharacter);

	UFUNCTION()
	void HandleSegmentDepletedRumble(int32 SegmentIndex);

	UFUNCTION()
	void HandlePickupRumble();

	UFUNCTION()
	void HandleThrowRumble();

	UFUNCTION()
	void HandleOwnDeathRumble(FVector DeathLocation);

	UPROPERTY()
	TObjectPtr<APlayerCharacter> ControlledCharacter;

	UPROPERTY()
	TObjectPtr<UPlayerTimerWidget> TimerWidget;
};
