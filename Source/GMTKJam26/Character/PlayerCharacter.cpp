#include "PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/DashComponent.h"
#include "Components/PickupComponent.h"
#include "Components/ThrowComponent.h"
#include "Components/PlayerTimerComponent.h"
#include "Components/PushComponent.h"
#include "Components/RespawnComponent.h"
#include "Components/DropComponent.h"

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

	if (PickupComponent->IsHoldingItem())
	{
		if (ThrowComponent)
		{
			ThrowComponent->ThrowHeldItem();
		}
		return;
	}

	if (PushComponent && PushComponent->TryShoveNearbyPlayer())
	{
		return;
	}

	PickupComponent->TryPickup();
}

void APlayerCharacter::RequestDrop()
{
	if (PickupComponent)
	{
		PickupComponent->DropItem();
	}
}
