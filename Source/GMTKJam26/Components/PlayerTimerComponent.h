#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerTimerComponent.generated.h"

class ARobotPart;

USTRUCT(BlueprintType)
struct FTimerSegment
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Timer")
	TSubclassOf<ARobotPart> ItemClass;

	UPROPERTY(BlueprintReadOnly, Category = "Timer")
	float MaxDuration = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Timer")
	float RemainingTime = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Timer")
	bool bIsDelivered = false;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSegmentDepletedSignature, int32, SegmentIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemAddedSignature, TSubclassOf<ARobotPart>, ItemClass);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTimerExpiredSignature);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GMTKJAM26_API UPlayerTimerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPlayerTimerComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Timer")
	bool AddItem(ARobotPart* Item);

	UFUNCTION(BlueprintPure, Category = "Timer")
	TArray<FTimerSegment> GetSegments() const { return Segments; }

	UFUNCTION(BlueprintPure, Category = "Timer")
	float GetTotalRemainingTime() const;

	UFUNCTION(BlueprintPure, Category = "Timer")
	int32 GetActiveSegmentIndex() const { return FindActiveSegmentIndex(); }

	UPROPERTY(BlueprintAssignable, Category = "Timer")
	FOnSegmentDepletedSignature OnSegmentDepleted;

	UPROPERTY(BlueprintAssignable, Category = "Timer")
	FOnItemAddedSignature OnItemAdded;

	UPROPERTY(BlueprintAssignable, Category = "Timer")
	FOnTimerExpiredSignature OnTimerExpired;

protected:
	virtual void InitializeComponent() override;

	UPROPERTY(EditDefaultsOnly, Category = "Timer")
	float CoreDuration = 120.f;

	UPROPERTY(EditDefaultsOnly, Category = "Timer")
	TArray<TSubclassOf<ARobotPart>> ItemOrder;

private:
	int32 FindActiveSegmentIndex() const;

	TArray<FTimerSegment> Segments;

	bool bExpired = false;
};
