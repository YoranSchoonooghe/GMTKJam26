#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemPlacementActor.generated.h"

class UBoxComponent;

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

	UFUNCTION()
	void OnBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

public:	
	virtual void Tick(float DeltaTime) override;

private:
	bool _bHasItem{ false };
};
