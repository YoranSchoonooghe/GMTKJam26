#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ThrowComponent.generated.h"

class UPickupComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnItemThrownSignature);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GMTKJAM26_API UThrowComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UThrowComponent();

	UFUNCTION(BlueprintCallable, Category = "Throw")
	void ThrowHeldItem();

	UPROPERTY(BlueprintAssignable, Category = "Throw")
	FOnItemThrownSignature OnItemThrown;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Throw")
	float ThrowForce = 1000.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Throw")
	float ThrowAngle = 20.f;

private:
	UPROPERTY()
	TObjectPtr<UPickupComponent> PickupComponent;
};
