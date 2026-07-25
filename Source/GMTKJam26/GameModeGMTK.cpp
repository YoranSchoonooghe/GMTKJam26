#include "GameModeGMTK.h"
#include "Engine/GameInstance.h"
#include "Engine/GameViewportClient.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Components/PlayerTimerComponent.h"
#include "Components/RespawnComponent.h"
#include "Menu/MenuFlowSubsystem.h"
#include "Menu/States/MenuStateBase.h"
#include "Character/PlayerControllerBase.h"
#include "Interaction/ItemPlacementActor.h"

void AGameModeGMTK::BeginPlay()
{
	Super::BeginPlay();

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (UMenuFlowSubsystem* MenuFlow = GameInstance->GetSubsystem<UMenuFlowSubsystem>())
		{
			NumberOfLocalPlayers = MenuFlow->GetDesiredPlayerCount();
		}

		while (GameInstance->GetNumLocalPlayers() < NumberOfLocalPlayers)
		{
			FString Error;
			if (!GameInstance->CreateLocalPlayer(-1, Error, true))
			{
				UE_LOG(LogTemp, Warning, TEXT("AGameModeGMTK: Failed to create local player - %s"), *Error);
				break;
			}
		}

		if (UGameViewportClient* ViewportClient = GameInstance->GetGameViewportClient())
		{
			ViewportClient->SetForceDisableSplitscreen(true);
		}
	}

	BindPlayerTimerEvents();
	BindPlayerRespawnEvents();
}

void AGameModeGMTK::BindPlayerTimerEvents()
{
	bool bAllBound = true;

	for (int32 PlayerIndex = 0; PlayerIndex < NumberOfLocalPlayers; ++PlayerIndex)
	{
		APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, PlayerIndex);
		if (!PlayerPawn)
		{
			bAllBound = false;
			continue;
		}

		UPlayerTimerComponent* Timer = PlayerPawn->FindComponentByClass<UPlayerTimerComponent>();
		if (!Timer)
		{
			continue;
		}

		switch (PlayerIndex)
		{
		case 0: Timer->OnTimerExpired.AddUniqueDynamic(this, &AGameModeGMTK::HandlePlayer1TimerExpired); break;
		case 1: Timer->OnTimerExpired.AddUniqueDynamic(this, &AGameModeGMTK::HandlePlayer2TimerExpired); break;
		case 2: Timer->OnTimerExpired.AddUniqueDynamic(this, &AGameModeGMTK::HandlePlayer3TimerExpired); break;
		case 3: Timer->OnTimerExpired.AddUniqueDynamic(this, &AGameModeGMTK::HandlePlayer4TimerExpired); break;
		default: break;
		}
	}

	if (!bAllBound)
	{
		GetWorldTimerManager().SetTimer(BindTimerEventsRetryHandle, this, &AGameModeGMTK::BindPlayerTimerEvents, 0.2f, false);
	}
}

void AGameModeGMTK::BindPlayerRespawnEvents()
{
	bool bAllBound = true;

	for (int32 PlayerIndex = 0; PlayerIndex < NumberOfLocalPlayers; ++PlayerIndex)
	{
		APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, PlayerIndex);
		URespawnComponent* Respawn = PlayerPawn ? PlayerPawn->FindComponentByClass<URespawnComponent>() : nullptr;

		if (!Respawn)
		{
			bAllBound = false;
			continue;
		}

		for (int32 OtherIndex = 0; OtherIndex < NumberOfLocalPlayers; ++OtherIndex)
		{
			if (OtherIndex == PlayerIndex)
			{
				continue;
			}

			APawn* OtherPawn = UGameplayStatics::GetPlayerPawn(this, OtherIndex);
			APlayerControllerBase* OtherPC = OtherPawn ? Cast<APlayerControllerBase>(OtherPawn->GetController()) : nullptr;
			if (!OtherPC)
			{
				bAllBound = false;
				continue;
			}

			Respawn->OnPlayerDied.AddUniqueDynamic(OtherPC, &APlayerControllerBase::NotifyOpponentDied);
		}
	}

	if (!bAllBound)
	{
		GetWorldTimerManager().SetTimer(BindRespawnEventsRetryHandle, this, &AGameModeGMTK::BindPlayerRespawnEvents, 0.2f, false);
	}
}

void AGameModeGMTK::HandlePlayer1TimerExpired()
{
	HandleGameOver(0);
}

void AGameModeGMTK::HandlePlayer2TimerExpired()
{
	HandleGameOver(1);
}

void AGameModeGMTK::HandlePlayer3TimerExpired()
{
	HandleGameOver(2);
}

void AGameModeGMTK::HandlePlayer4TimerExpired()
{
	HandleGameOver(3);
}

void AGameModeGMTK::HandleGameOver(int32 LosingPlayerIndex)
{
	if (bGameOverTriggered || !GameOverStateClass)
	{
		return;
	}

	bGameOverTriggered = true;

	PendingWinningPlayerIndex = (NumberOfLocalPlayers == 2) ? ((LosingPlayerIndex == 0) ? 1 : 0) : -1;

	APawn* LosingPawn = UGameplayStatics::GetPlayerPawn(this, LosingPlayerIndex);
	if (LosingPawn)
	{
		if (AController* LosingController = LosingPawn->GetController())
		{
			LosingController->UnPossess();
		}
	}

	OnGameOverSequenceStarted.Broadcast(LosingPawn, LosingPlayerIndex);

	ExplodeRobotParts(LosingPlayerIndex);

	GetWorldTimerManager().SetTimer(GameOverDelayTimerHandle, this, &AGameModeGMTK::ShowGameOverMenu, GameOverDelaySeconds, false);
}

void AGameModeGMTK::ExplodeRobotParts(int32 LosingPlayerIndex)
{
	TArray<AActor*> PlacementActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AItemPlacementActor::StaticClass(), PlacementActors);

	for (AActor* Actor : PlacementActors)
	{
		if (AItemPlacementActor* PlacementActor = Cast<AItemPlacementActor>(Actor))
		{
			if (PlacementActor->GetPlayerIndex() == LosingPlayerIndex)
			{
				PlacementActor->ExplodeAttachedItem();
			}
		}
	}
}

void AGameModeGMTK::ShowGameOverMenu()
{
	OnGameOverSequenceEnded.Broadcast(PendingWinningPlayerIndex);

	if (UMenuFlowSubsystem* Flow = GetGameInstance() ? GetGameInstance()->GetSubsystem<UMenuFlowSubsystem>() : nullptr)
	{
		Flow->SetGameOverWinnerIndex(PendingWinningPlayerIndex);
		Flow->PushState(GameOverStateClass);
	}
}

AActor* AGameModeGMTK::ChoosePlayerStart_Implementation(AController* Player)
{
	TArray<AActor*> PlayerStarts;
	for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
	{
		PlayerStarts.Add(*It);
	}

	if (PlayerStarts.Num() > 0)
	{
		AActor* ChosenStart = PlayerStarts[NextPlayerStartIndex % PlayerStarts.Num()];
		NextPlayerStartIndex++;
		return ChosenStart;
	}

	return Super::ChoosePlayerStart_Implementation(Player);
}
