#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PushComponent.generated.h"

class UCapsuleComponent;
class ACharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnKnockbackSignature, FVector, SourceLocation);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnShoveSignature, ACharacter*, ShovedCharacter);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GMTKJAM26_API UPushComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPushComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Push")
	void ApplyKnockback(const FVector& SourceLocation, float Force, float UpwardForce);

	UFUNCTION(BlueprintCallable, Category = "Push")
	bool TryShoveNearbyPlayer();

	UFUNCTION(BlueprintCallable, Category = "Push")
	TArray<ACharacter*> GetOverlappingCharacters() const;

	UFUNCTION(BlueprintCallable, Category = "Push")
	void SetZoneSize(float InRadiusPadding, float InHalfHeightPadding);

	UPROPERTY(BlueprintAssignable, Category = "Push|Events")
	FOnKnockbackSignature OnKnockback;

	UPROPERTY(BlueprintAssignable, Category = "Push|Events")
	FOnShoveSignature OnShove;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Push")
	float ZoneRadiusPadding = 30.f;

	UPROPERTY(EditDefaultsOnly, Category = "Push")
	float ZoneHalfHeightPadding = 20.f;

	UPROPERTY(EditDefaultsOnly, Category = "Push")
	float ShoveForce = 700.f;

	UPROPERTY(EditDefaultsOnly, Category = "Push")
	float ShoveUpwardForce = 150.f;

	UPROPERTY(EditDefaultsOnly, Category = "Push|Passive")
	float PassivePushSpeedFactor = 0.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Push")
	float KnockbackSourceOffset = 20.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Push|Debug")
	bool bDrawDebugZone = false;

private:
	void ApplyPassivePush(float DeltaTime);
	void RebuildZoneSize();

	UPROPERTY()
	TObjectPtr<UCapsuleComponent> PushZone;

	TWeakObjectPtr<ACharacter> OwningCharacter;
};
