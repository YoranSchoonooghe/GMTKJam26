#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameModeGMTK.generated.h"

UCLASS()
class GMTKJAM26_API AGameModeGMTK : public AGameModeBase
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	UPROPERTY(EditDefaultsOnly, Category = "Players")
	int32 NumberOfLocalPlayers = 2;

private:
	int32 NextPlayerStartIndex = 0;
};
