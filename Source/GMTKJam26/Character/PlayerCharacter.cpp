#include "PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/DashComponent.h"
#include "Components/PickupComponent.h"
#include "Components/ThrowComponent.h"
#include "Components/PlayerTimerComponent.h"
#include "Components/PushComponent.h"
#include "Components/RespawnComponent.h"
#include "Components/DropComponent.h"
#include "Animation/AnimMontage.h"

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 500.f, 0.f);
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;

	GetCharacterMovement()->bEnablePhysicsInteraction = true;

	DashComponent = CreateDefaultSubobject<UDashComponent>(TEXT("DashComponent"));
	PickupComponent = CreateDefaultSubobject<UPickupComponent>(TEXT("PickupComponent"));
	ThrowComponent = CreateDefaultSubobject<UThrowComponent>(TEXT("ThrowComponent"));
	TimerComponent = CreateDefaultSubobject<UPlayerTimerComponent>(TEXT("TimerComponent"));
	PushComponent = CreateDefaultSubobject<UPushComponent>(TEXT("PushComponent"));
	RespawnComponent = CreateDefaultSubobject<URespawnComponent>(TEXT("RespawnComponent"));
	DropComponent = CreateDefaultSubobject<UDropComponent>(TEXT("DropComponent"));
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	PickupComponent->OnItemPickedUp.AddDynamic(this, &APlayerCharacter::UpdateWalkSpeed);
	PickupComponent->OnItemDropped.AddDynamic(this, &APlayerCharacter::UpdateWalkSpeed);
	ThrowComponent->OnItemThrown.AddDynamic(this, &APlayerCharacter::UpdateWalkSpeed);
	DropComponent->OnItemLost.AddDynamic(this, &APlayerCharacter::UpdateWalkSpeed);

	RespawnComponent->OnPlayerDied.AddDynamic(this, &APlayerCharacter::HandlePlayerDied);

	ThrowComponent->OnItemThrown.AddDynamic(this, &APlayerCharacter::PlayThrowMontage);

	if (PushComponent)
	{
		PushComponent->OnKnockback.AddDynamic(this, &APlayerCharacter::PlayPushHitMontage);
	}
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APlayerCharacter::UpdateWalkSpeed()
{
	if (PickupComponent->IsHoldingItem())
	{
		GetCharacterMovement()->MaxWalkSpeed = CarryWalkSpeed;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
	}
}

void APlayerCharacter::HandlePlayerDied(FVector DeathLocation)
{
	if (TimerComponent)
	{
		TimerComponent->ForfeitActiveSegment();
	}
}

void APlayerCharacter::ExecutePickupGrab()
{
	if (PickupComponent)
	{
		PickupComponent->TryPickup();
	}
}

void APlayerCharacter::PlayThrowMontage()
{
	if (ThrowMontage)
	{
		PlayAnimMontage(ThrowMontage);
	}
}

void APlayerCharacter::PlayPushHitMontage(FVector SourceLocation)
{
	if (PushHitMontage)
	{
		PlayAnimMontage(PushHitMontage);
	}
}

void APlayerCharacter::Move(const FVector2D& MovementVector)
{
	if (DashComponent && DashComponent->IsDashing())
	{
		return;
	}

	AddMovementInput(FVector::ForwardVector, MovementVector.Y);
	AddMovementInput(FVector::RightVector, MovementVector.X);
}

void APlayerCharacter::RequestDash()
{
	if (DashComponent)
	{
		DashComponent->StartDash();
	}
}

void APlayerCharacter::RequestInteract()
{
	if (!PickupComponent)
	{
		return;
	}

	if (GetWorldTimerManager().IsTimerActive(InteractCooldownTimerHandle))
	{
		return;
	}

	GetWorldTimerManager().SetTimer(InteractCooldownTimerHandle, InteractCooldown, false);

	if (PickupComponent->IsHoldingItem())
	{
		if (ThrowComponent)
		{
			ThrowComponent->ThrowHeldItem();
		}
		return;
	}

	if (PickupComponent->HasNearbyItem())
	{
		if (PickupMontage)
		{
			PlayAnimMontage(PickupMontage);
		}
		else
		{
			PickupComponent->TryPickup();
		}
		return;
	}

	if (PunchMontage)
	{
		PlayAnimMontage(PunchMontage);
	}

	if (PushComponent)
	{
		PushComponent->StartPunch();
	}
}

void APlayerCharacter::RequestDrop()
{
	if (PickupComponent)
	{
		PickupComponent->DropItem();
	}
}
