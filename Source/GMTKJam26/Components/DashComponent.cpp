#include "DashComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UDashComponent::UDashComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UDashComponent::BeginPlay()
{
	Super::BeginPlay();

	OwningCharacter = Cast<ACharacter>(GetOwner());
}

void UDashComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsDashing && OwningCharacter.IsValid())
	{
		OwningCharacter->GetCharacterMovement()->Velocity = CurrentDashDirection * DashSpeed;
	}
}

void UDashComponent::StartDash()
{
	if (!bCanDash || bIsDashing || !OwningCharacter.IsValid())
	{
		return;
	}

	FVector DashDirection = OwningCharacter->GetLastMovementInputVector();
	if (DashDirection.IsNearlyZero())
	{
		DashDirection = OwningCharacter->GetActorForwardVector();
	}
	CurrentDashDirection = DashDirection.GetSafeNormal2D();

	bIsDashing = true;
	bCanDash = false;

	UCharacterMovementComponent* MovementComponent = OwningCharacter->GetCharacterMovement();
	MovementComponent->SetMovementMode(MOVE_Flying);
	MovementComponent->Velocity = CurrentDashDirection * DashSpeed;

	OwningCharacter->GetWorldTimerManager().SetTimer(DashDurationTimerHandle, this, &UDashComponent::StopDash, DashDuration, false);
	OwningCharacter->GetWorldTimerManager().SetTimer(DashCooldownTimerHandle, this, &UDashComponent::ResetDash, DashCooldown, false);
}

void UDashComponent::StopDash()
{
	bIsDashing = false;

	if (OwningCharacter.IsValid())
	{
		OwningCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Falling);
	}
}

void UDashComponent::ResetDash()
{
	bCanDash = true;
}
