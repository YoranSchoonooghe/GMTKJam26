#include "RespawnComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "UI/RespawnCountdownWidget.h"
#include "GMTKJam26/Events/EventBusSubsystem.h"
#include "Components/PlayerTimerComponent.h"

URespawnComponent::URespawnComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void URespawnComponent::BeginPlay()
{
	Super::BeginPlay();

	OwningCharacter = Cast<ACharacter>(GetOwner());
	if (OwningCharacter.IsValid())
	{
		SpawnLocation = OwningCharacter->GetActorLocation();
		SpawnRotation = OwningCharacter->GetActorRotation();
	}
}

void URespawnComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsDead || !OwningCharacter.IsValid())
	{
		return;
	}

	if (OwningCharacter->GetActorLocation().Z < VoidZThreshold)
	{
		HandleFallIntoVoid();
	}
}

void URespawnComponent::HandleFallIntoVoid()
{
	if (bIsDead || !OwningCharacter.IsValid())
	{
		return;
	}

	bIsDead = true;

	if (UPlayerTimerComponent* TimerComponent = OwningCharacter->FindComponentByClass<UPlayerTimerComponent>())
	{
		TimerComponent->ApplyDeathPenalty();
	}

	const FVector DeathLocation = OwningCharacter->GetActorLocation();

	OwningCharacter->SetActorHiddenInGame(true);
	OwningCharacter->SetActorEnableCollision(false);

	if (UCharacterMovementComponent* Movement = OwningCharacter->GetCharacterMovement())
	{
		Movement->StopMovementImmediately();
		Movement->DisableMovement();
	}

	if (!CountdownWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("URespawnComponent (%s): CountdownWidgetClass is not set - no respawn countdown will be shown."), *GetNameSafe(OwningCharacter.Get()));
	}
	else if (APlayerController* PC = Cast<APlayerController>(OwningCharacter->GetController()))
	{
		CountdownWidget = CreateWidget<URespawnCountdownWidget>(PC, CountdownWidgetClass);
		if (CountdownWidget)
		{
			CountdownWidget->StartCountdown(DeathLocation, RespawnDelay);
			CountdownWidget->AddToViewport();
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("URespawnComponent (%s): no valid PlayerController at time of death - no respawn countdown will be shown."), *GetNameSafe(OwningCharacter.Get()));
	}

	GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle, this, &URespawnComponent::Respawn, RespawnDelay, false);

	OnPlayerDied.Broadcast(DeathLocation);
	GetWorld()->GetSubsystem<UEventBusSubsystem>()->SendEvent("OnPlayerDied");
}

void URespawnComponent::Respawn()
{
	if (!OwningCharacter.IsValid())
	{
		return;
	}

	if (CountdownWidget)
	{
		CountdownWidget->RemoveFromParent();
		CountdownWidget = nullptr;
	}

	OwningCharacter->SetActorEnableCollision(true);
	OwningCharacter->SetActorLocation(SpawnLocation);
	OwningCharacter->SetActorRotation(SpawnRotation);
	OwningCharacter->SetActorHiddenInGame(false);

	if (UCharacterMovementComponent* Movement = OwningCharacter->GetCharacterMovement())
	{
		Movement->SetMovementMode(MOVE_Walking);
	}

	bIsDead = false;

	OnPlayerRespawned.Broadcast();
}
