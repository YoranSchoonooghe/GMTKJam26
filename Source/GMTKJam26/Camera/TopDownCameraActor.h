#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TopDownCameraActor.generated.h"

class UCameraComponent;

UCLASS()
class GMTKJAM26_API ATopDownCameraActor : public AActor
{
	GENERATED_BODY()

public:
	ATopDownCameraActor();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> CameraComponent;
};
