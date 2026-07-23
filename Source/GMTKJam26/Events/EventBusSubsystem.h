#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "EventBusSubsystem.generated.h"

UCLASS()
class GMTKJAM26_API UEventBusSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	void SendEvent(FName EventName);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRobotPartAttached);
	UPROPERTY()
	FOnRobotPartAttached OnRobotPartAttached;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerDied);
	UPROPERTY()
	FOnPlayerDied OnPlayerDied;

};
