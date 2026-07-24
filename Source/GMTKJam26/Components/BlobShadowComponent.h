#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BlobShadowComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GMTKJAM26_API UBlobShadowComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UBlobShadowComponent();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blob Shadow")
	TObjectPtr<UMaterialInterface> ShadowMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blob Shadow")
	float MaxTraceDistance{ 1000.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blob Shadow")
	FVector DecalSize{ FVector(2.0f, 32.0f, 32.0f) };

	UPROPERTY(EditAnywhere, Category = "Blob Shadow")
	FName OpacityParamName{ FName("Opacity") };

	UPROPERTY(EditAnywhere, Category = "Blob Shadow")
	TEnumAsByte<ECollisionChannel> TraceChannel{ ECC_Visibility };

private:
	void UpdateShadow();

	UPROPERTY(Transient)
	TObjectPtr<UDecalComponent> DecalComponent;

	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> DynamicMaterial;
};
