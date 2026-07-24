#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StunComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStunStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStunEnded);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GMTKJAM26_API UStunComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UStunComponent();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stun")
	float StunDuration{ 1.0f };

	UPROPERTY()
	APlayerController* OwnerController{ nullptr };

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Stun")
	void Stun();
	UFUNCTION(BlueprintCallable, Category = "Stun")
	bool IsStunned() const { return _bIsStunned; }

	UPROPERTY(BlueprintAssignable, Category = "Stun")
	FOnStunStarted OnStunStarted;
	UPROPERTY(BlueprintAssignable, Category = "Stun")
	FOnStunEnded OnStunEnded;
	
private:
	void UpdateStun(float DeltaTime);

	bool _bIsStunned{ false };
	float _elapsedStunTime{ 0.0f };
};
