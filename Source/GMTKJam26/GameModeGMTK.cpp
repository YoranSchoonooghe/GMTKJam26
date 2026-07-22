#include "GameModeGMTK.h"
#include "Engine/GameInstance.h"
#include "Engine/GameViewportClient.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerStart.h"

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
