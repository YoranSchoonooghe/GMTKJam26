#include "EventBusSubsystem.h"

void UEventBusSubsystem::SendEvent(FName EventName)
{
	if (EventName == TEXT("OnPlayerDied"))
		OnPlayerDied.Broadcast();
	else if (EventName == TEXT("OnRobotPartAttached"))
		OnRobotPartAttached.Broadcast();
	else if (EventName == TEXT("OnRobotTimeExpired"))
		OnRobotTimeExpired.Broadcast();
}
