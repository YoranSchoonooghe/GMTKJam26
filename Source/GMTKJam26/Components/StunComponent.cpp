#include "StunComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UStunComponent::UStunComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}


void UStunComponent::BeginPlay()
{
	Super::BeginPlay();

	auto* pOwnerCharacter = Cast<ACharacter>(GetOwner());
	if (pOwnerCharacter)
	{
		OwnerController = Cast<APlayerController>(pOwnerCharacter->GetController());
	}
}


void UStunComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateStun(DeltaTime);
}

void UStunComponent::Stun()
{
	if (_bIsStunned) return;

	if (OwnerController)
	{
		OwnerController->SetIgnoreMoveInput(true);
	}

	_bIsStunned = true;
	_elapsedStunTime = 0.0f;

	OnStunStarted.Broadcast();
}

void UStunComponent::UpdateStun(float DeltaTime)
{
	if (!_bIsStunned) return;

	_elapsedStunTime += DeltaTime;

	if (_elapsedStunTime >= StunDuration)
	{
		_elapsedStunTime = 0.0f;
		_bIsStunned = false;

		if (OwnerController)
		{
			OwnerController->SetIgnoreMoveInput(false);
		}

		OnStunEnded.Broadcast();
	}
}

