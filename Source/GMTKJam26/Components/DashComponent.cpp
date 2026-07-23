#include "DashComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PushComponent.h"

UDashComponent::UDashComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UDashComponent::BeginPlay()
{
	Super::BeginPlay();

	OwningCharacter = Cast<ACharacter>(GetOwner());

	if (OwningCharacter.IsValid())
	{
		OwningCharacter->OnActorHit.AddDynamic(this, &UDashComponent::OnCharacterHit);
	}
}

void UDashComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bIsDashing || !OwningCharacter.IsValid())
	{
		return;
	}

	OwningCharacter->GetCharacterMovement()->Velocity = CurrentDashDirection * DashSpeed;

	// Checked every frame rather than relying solely on OnActorHit - a fast-moving dash can sweep
	// straight past another capsule in a single frame without ever generating a blocking hit.
	if (!bHasPushedThisDash)
	{
		if (UPushComponent* MyPush = OwningCharacter->FindComponentByClass<UPushComponent>())
		{
			for (ACharacter* NearbyCharacter : MyPush->GetOverlappingCharacters())
			{
				if (UPushComponent* OtherPush = NearbyCharacter->FindComponentByClass<UPushComponent>())
				{
					bHasPushedThisDash = true;
					OtherPush->ApplyKnockback(OwningCharacter->GetActorLocation(), PushForce, PushUpwardForce);
					break;
				}
			}
		}
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
	bHasPushedThisDash = false;

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

void UDashComponent::OnCharacterHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!bIsDashing || bHasPushedThisDash || !OwningCharacter.IsValid())
	{
		return;
	}

	ACharacter* OtherCharacter = Cast<ACharacter>(OtherActor);
	if (!OtherCharacter)
	{
		return;
	}

	if (UPushComponent* OtherPush = OtherCharacter->FindComponentByClass<UPushComponent>())
	{
		bHasPushedThisDash = true;
		OtherPush->ApplyKnockback(OwningCharacter->GetActorLocation(), PushForce, PushUpwardForce);
	}
}
