#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FallingStair.generated.h"

UENUM(BlueprintType)
enum class EStairState : uint8
{
	Idle,
	Shaking,
	Falling,
	Cooldown,
	Resetting
};

UCLASS()
class GMTKJAM26_API AFallingStair : public AActor
{
	GENERATED_BODY()
	
public:	
	AFallingStair();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> StairMesh;

	UPROPERTY(EditAnywhere, Category = "Falling Stair")
	float TimeToFall{ 5.0f };
	UPROPERTY(EditAnywhere, Category = "Falling Stair")
	float FallDistance{ 1000.0f };

	UPROPERTY(EditAnywhere, Category = "Falling Stair")
	float FloatAmplitude{ 10.0f };
	UPROPERTY(EditAnywhere, Category = "Falling Stair")
	float FloatFrequency{ 1.0f };

	UPROPERTY(EditAnywhere, Category = "Falling Stair")
	float ShakeDuration{ 1.0f };
	UPROPERTY(EditAnywhere, Category = "Falling Stair")
	float ShakeIntensity{ 1.0f };

	UPROPERTY(EditAnywhere, Category = "Falling Stair")
	float GravityMultiplier{ 1.0f };

	UPROPERTY(EditAnywhere, Category = "Falling Stair")
	float ResetDuration{ 1.0f };

public:	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void Reset();

private:
	void UpdateIdleState(float DeltaTime);
	void UpdateShakingState(float DeltaTime);
	void UpdateFallingState(float DeltaTime);
	void UpdateResettingState(float DeltaTime);

	EStairState _state{ EStairState::Idle };
	FVector _startLocation;
	FVector _cooldownLocation;

	float _elapsedTimeInState{ 0.0f };

	float _phaseOffset;
};
