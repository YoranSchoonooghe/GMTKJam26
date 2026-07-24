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

void AGameModeGMTK::BeginPlay()
{
	Super::BeginPlay();

	if (UGameInstance* GameInstance = GetGameInstance())
	{
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
	APawn* Player1Pawn = UGameplayStatics::GetPlayerPawn(this, 0);
	APawn* Player2Pawn = UGameplayStatics::GetPlayerPawn(this, 1);

	bool bBothBound = true;

	if (Player1Pawn)
	{
		if (UPlayerTimerComponent* Timer = Player1Pawn->FindComponentByClass<UPlayerTimerComponent>())
		{
			Timer->OnTimerExpired.AddUniqueDynamic(this, &AGameModeGMTK::HandlePlayer1TimerExpired);
		}
	}
	else
	{
		bBothBound = false;
	}

	if (Player2Pawn)
	{
		if (UPlayerTimerComponent* Timer = Player2Pawn->FindComponentByClass<UPlayerTimerComponent>())
		{
			Timer->OnTimerExpired.AddUniqueDynamic(this, &AGameModeGMTK::HandlePlayer2TimerExpired);
		}
	}
	else
	{
		bBothBound = false;
	}

	if (!bBothBound)
	{
		GetWorldTimerManager().SetTimer(BindTimerEventsRetryHandle, this, &AGameModeGMTK::BindPlayerTimerEvents, 0.2f, false);
	}
}

void AGameModeGMTK::BindPlayerRespawnEvents()
{
	APawn* Player1Pawn = UGameplayStatics::GetPlayerPawn(this, 0);
	APawn* Player2Pawn = UGameplayStatics::GetPlayerPawn(this, 1);

	URespawnComponent* Respawn1 = Player1Pawn ? Player1Pawn->FindComponentByClass<URespawnComponent>() : nullptr;
	URespawnComponent* Respawn2 = Player2Pawn ? Player2Pawn->FindComponentByClass<URespawnComponent>() : nullptr;

	APlayerControllerBase* PC1 = Player1Pawn ? Cast<APlayerControllerBase>(Player1Pawn->GetController()) : nullptr;
	APlayerControllerBase* PC2 = Player2Pawn ? Cast<APlayerControllerBase>(Player2Pawn->GetController()) : nullptr;

	bool bBothBound = true;

	if (Respawn1 && PC2)
	{
		Respawn1->OnPlayerDied.AddUniqueDynamic(PC2, &APlayerControllerBase::NotifyOpponentDied);
	}
	else
	{
		bBothBound = false;
	}

	if (Respawn2 && PC1)
	{
		Respawn2->OnPlayerDied.AddUniqueDynamic(PC1, &APlayerControllerBase::NotifyOpponentDied);
	}
	else
	{
		bBothBound = false;
	}

	if (!bBothBound)
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

void AGameModeGMTK::HandleGameOver(int32 LosingPlayerIndex)
{
	if (bGameOverTriggered || !GameOverStateClass)
	{
		return;
	}

	bGameOverTriggered = true;
	PendingWinningPlayerIndex = (LosingPlayerIndex == 0) ? 1 : 0;

	APawn* LosingPawn = UGameplayStatics::GetPlayerPawn(this, LosingPlayerIndex);
	if (LosingPawn)
	{
		if (AController* LosingController = LosingPawn->GetController())
		{
			LosingController->UnPossess();
		}
	}

	OnGameOverSequenceStarted.Broadcast(LosingPawn, LosingPlayerIndex);

	GetWorldTimerManager().SetTimer(GameOverDelayTimerHandle, this, &AGameModeGMTK::ShowGameOverMenu, GameOverDelaySeconds, false);
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
