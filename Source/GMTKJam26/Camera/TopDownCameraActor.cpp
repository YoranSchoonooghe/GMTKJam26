#include "TopDownCameraActor.h"
#include "Camera/CameraComponent.h"
#include "Components/CameraShakeComponent.h"
#include "GMTKJam26/Events/EventBusSubsystem.h"

ATopDownCameraActor::ATopDownCameraActor()
{
	PrimaryActorTick.bCanEverTick = false;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	RootComponent = CameraComponent;

	CameraComponent->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f));

	CameraShakeComponent = CreateDefaultSubobject<UCameraShakeComponent>(TEXT("CameraShakeComponent"));
}

void ATopDownCameraActor::BeginPlay()
{
	Super::BeginPlay();

	auto* pEventBus = GetWorld()->GetSubsystem<UEventBusSubsystem>();
	pEventBus->OnRobotPartAttached.AddDynamic(this, &ATopDownCameraActor::RequestItemPlacementCameraShake);
	pEventBus->OnPlayerDied.AddDynamic(this, &ATopDownCameraActor::RequestDeathCameraShake);
}

void ATopDownCameraActor::RequestDeathCameraShake()
{
	CameraShakeComponent->ShakeCamera(DeathCameraShake);
}

void ATopDownCameraActor::RequestItemPlacementCameraShake()
{
	CameraShakeComponent->ShakeCamera(ItemPlacementCameraShake);
}
