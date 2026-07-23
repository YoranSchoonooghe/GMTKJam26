#include "AutoHideComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "RespawnComponent.h"

UAutoHideComponent::UAutoHideComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UAutoHideComponent::BeginPlay()
{
	Super::BeginPlay();

	OwningActor = GetOwner();

	BindToPlayerRespawnEvents();

	GetWorld()->GetTimerManager().SetTimer(HideTimerHandle, this, &UAutoHideComponent::Hide, HideDelay, false);
}

void UAutoHideComponent::BindToPlayerRespawnEvents()
{
	int32 BoundCount = 0;

	for (int32 PlayerIndex = 0; PlayerIndex < 2; ++PlayerIndex)
	{
		if (APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, PlayerIndex))
		{
			if (URespawnComponent* Respawn = PlayerPawn->FindComponentByClass<URespawnComponent>())
			{
				Respawn->OnPlayerDied.AddUniqueDynamic(this, &UAutoHideComponent::OnAnyPlayerDied);
				++BoundCount;
			}
		}
	}

	if (BoundCount < 2)
	{
		GetWorld()->GetTimerManager().SetTimer(BindRetryTimerHandle, this, &UAutoHideComponent::BindToPlayerRespawnEvents, 0.2f, false);
	}
}

void UAutoHideComponent::OnAnyPlayerDied(FVector DeathLocation)
{
	Show();
}

void UAutoHideComponent::Show()
{
	if (!OwningActor.IsValid())
	{
		return;
	}

	OwningActor->SetActorHiddenInGame(false);

	if (bToggleCollision)
	{
		OwningActor->SetActorEnableCollision(true);
	}

	GetWorld()->GetTimerManager().SetTimer(HideTimerHandle, this, &UAutoHideComponent::Hide, HideDelay, false);
}

void UAutoHideComponent::Hide()
{
	if (!OwningActor.IsValid())
	{
		return;
	}

	OwningActor->SetActorHiddenInGame(true);

	if (bToggleCollision)
	{
		OwningActor->SetActorEnableCollision(false);
	}
}
