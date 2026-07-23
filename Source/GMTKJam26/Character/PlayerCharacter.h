#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

class UDashComponent;
class UPickupComponent;
class UThrowComponent;
class UPlayerTimerComponent;

UCLASS()
class GMTKJAM26_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();

	void Move(const FVector2D& MovementVector);
	void RequestDash();

	void RequestInteract();
	void RequestDrop();

	UFUNCTION(BlueprintPure, Category = "Timer")
	UPlayerTimerComponent* GetTimerComponent() const { return TimerComponent; }

	UPickupComponent* GetPickupComponent() { return PickupComponent; }
	UThrowComponent* GetThrowComponent() { return ThrowComponent; }

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dash", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDashComponent> DashComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPickupComponent> PickupComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UThrowComponent> ThrowComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Timer", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPlayerTimerComponent> TimerComponent;
};
