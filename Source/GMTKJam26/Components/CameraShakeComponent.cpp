#include "CameraShakeComponent.h"

UCameraShakeComponent::UCameraShakeComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}


void UCameraShakeComponent::BeginPlay()
{
	Super::BeginPlay();


}


void UCameraShakeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UCameraShakeComponent::ShakeCamera()
{
	if (auto* pPlayerController = GetWorld()->GetFirstPlayerController())
	{
		pPlayerController->PlayerCameraManager->StartCameraShake(CameraShake);
	}
}
