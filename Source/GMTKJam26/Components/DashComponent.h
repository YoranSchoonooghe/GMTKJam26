#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TimerManager.h"
#include "DashComponent.generated.h"

class ACharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDashStartedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDashEndedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDashHitCharacterSignature, ACharacter*, HitCharacter);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GMTKJAM26_API UDashComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDashComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Dash")
	void StartDash();

	UFUNCTION(BlueprintPure, Category = "Dash")
	bool IsDashing() const { return bIsDashing; }

	UFUNCTION(BlueprintPure, Category = "Dash")
	bool CanDash() const { return bCanDash; }

	UPROPERTY(BlueprintAssignable, Category = "Dash|Events")
	FOnDashStartedSignature OnDashStarted;

	UPROPERTY(BlueprintAssignable, Category = "Dash|Events")
	FOnDashEndedSignature OnDashEnded;

	UPROPERTY(BlueprintAssignable, Category = "Dash|Events")
	FOnDashHitCharacterSignature OnDashHitCharacter;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dash")
	float DashSpeed = 2500.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dash")
	float DashDuration = 0.2f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dash")
	float DashCooldown = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dash|Push")
	float PushForce = 900.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dash|Push")
	float PushUpwardForce = 200.f;

private:
	void StopDash();
	void ResetDash();
	void PushCharacter(ACharacter* OtherCharacter);

	UFUNCTION()
	void OnCharacterHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);

	TWeakObjectPtr<ACharacter> OwningCharacter;

	bool bIsDashing = false;
	bool bCanDash = true;
	bool bHasPushedThisDash = false;
	FVector CurrentDashDirection = FVector::ZeroVector;

	FTimerHandle DashDurationTimerHandle;
	FTimerHandle DashCooldownTimerHandle;
};
