#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RobotPart.h"
#include "ItemPlacementActor.generated.h"

class UBoxComponent;
class UWidgetComponent;
class APlayerCharacter;

UCLASS()
class GMTKJAM26_API AItemPlacementActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AItemPlacementActor();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<UBoxComponent> TriggerBox;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<UStaticMeshComponent> PreviewMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UWidgetComponent> InteractWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot Part")
	EPartType ExpectedPartType{ EPartType::Head };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target Player")
	int32 PlayerIndex{ 0 };

	UFUNCTION()
	void OnBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

public:	
	virtual void Tick(float DeltaTime) override;

private:
	UFUNCTION()
	void RetrievePlayerItem();
	UFUNCTION()
	void TryAttach();

	bool _bHasItem{ false };
	bool _bPlayerIsInRange{ false };
	ARobotPart* _targetItem{ nullptr };
	APlayerCharacter* _targetPlayer{ nullptr };
};
