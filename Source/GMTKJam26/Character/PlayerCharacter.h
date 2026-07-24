#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TimerManager.h"
#include "PlayerCharacter.generated.h"

class UDashComponent;
class UPickupComponent;
class UThrowComponent;
class UPlayerTimerComponent;
class UPushComponent;
class URespawnComponent;
class UDropComponent;
class UAnimMontage;

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

	void ExecutePickupGrab();

	UFUNCTION(BlueprintPure, Category = "Timer")
	UPlayerTimerComponent* GetTimerComponent() const { return TimerComponent; }

	UPickupComponent* GetPickupComponent() { return PickupComponent; }
	UThrowComponent* GetThrowComponent() { return ThrowComponent; }
	UDropComponent* GetDropComponent() { return DropComponent; }
	UPushComponent* GetPushComponent() { return PushComponent; }
	UDashComponent* GetDashComponent() { return DashComponent; }
	URespawnComponent* GetRespawnComponent() { return RespawnComponent; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
	float MaxWalkSpeed{ 600.0f };
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float CarryWalkSpeed{ 300.0f };

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Push", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPushComponent> PushComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Respawn", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<URespawnComponent> RespawnComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Drop", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDropComponent> DropComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> PickupMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> ThrowMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> PushHitMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> PunchMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	float InteractCooldown = 0.5f;

	UFUNCTION()
	void UpdateWalkSpeed();

	UFUNCTION()
	void HandlePlayerDied(FVector DeathLocation);

	UFUNCTION()
	void PlayThrowMontage();

	UFUNCTION()
	void PlayPushHitMontage(FVector SourceLocation);

	FTimerHandle InteractCooldownTimerHandle;
};
