#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DropComponent.generated.h"

class UPickupComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnItemLostSignature);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GMTKJAM26_API UDropComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UDropComponent();

	UPROPERTY(BlueprintAssignable, Category = "Throw")
	FOnItemLostSignature OnItemLost;

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Drop")
	void DropHeldItem();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drop")
	float LaunchPower{ 800.0f };
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drop")
	float LaunchPowerDeviation{ 100.0f };

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY()
	TObjectPtr<UPickupComponent> PickupComponent;

	UFUNCTION()
	void HandleKnockBack(FVector SourceLocation);
	UFUNCTION()
	void HandlePlayerDied(FVector DeathLocation);
};
