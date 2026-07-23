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
	pEventBus->OnRobotPartAttached.AddDynamic(this, &ATopDownCameraActor::RequestItemPlacementShake);
	pEventBus->OnPlayerDied.AddDynamic(this, &ATopDownCameraActor::RequestDeathShake);
	pEventBus->OnRobotTimeExpired.AddDynamic(this, &ATopDownCameraActor::RequestRobotTimeExpiredShake);
}

void ATopDownCameraActor::RequestDeathShake()
{
	CameraShakeComponent->ShakeCamera(DeathShake);
}

void ATopDownCameraActor::RequestItemPlacementShake()
{
	CameraShakeComponent->ShakeCamera(ItemPlacementShake);
}

void ATopDownCameraActor::RequestRobotTimeExpiredShake()
{
	CameraShakeComponent->ShakeCamera(RobotTimeExpiredShake);
}
