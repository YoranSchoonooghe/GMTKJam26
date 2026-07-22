#include "TopDownCameraActor.h"
#include "Camera/CameraComponent.h"

ATopDownCameraActor::ATopDownCameraActor()
{
	PrimaryActorTick.bCanEverTick = false;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	RootComponent = CameraComponent;

	CameraComponent->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f));
}
