#include "PlayerControllerBase.h"
#include "PlayerCharacter.h"
#include "Camera/TopDownCameraActor.h"
#include "UI/PlayerTimerWidget.h"
#include "Components/PlayerTimerComponent.h"
#include "Components/PushComponent.h"
#include "Components/DashComponent.h"
#include "Components/PickupComponent.h"
#include "Components/ThrowComponent.h"
#include "Components/RespawnComponent.h"
#include "Menu/MenuFlowSubsystem.h"
#include "Menu/States/MenuStateBase.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/ForceFeedbackEffect.h"
#include "GameFramework/ForceFeedbackParameters.h"

void APlayerControllerBase::BeginPlay()
{
	Super::BeginPlay();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		if (DefaultMappingContext)
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	if (AActor* StaticCamera = UGameplayStatics::GetActorOfClass(GetWorld(), ATopDownCameraActor::StaticClass()))
	{
		SetViewTarget(StaticCamera);
	}

	if (DefaultRootMenuState)
	{
		if (UMenuFlowSubsystem* Flow = GetGameInstance() ? GetGameInstance()->GetSubsystem<UMenuFlowSubsystem>() : nullptr)
		{
			if (!Flow->GetCurrentState())
			{
				Flow->SetRootState(DefaultRootMenuState);
			}
		}
	}
}

void APlayerControllerBase::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		if (MoveAction)
		{
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerControllerBase::Move);
		}

		if (JumpAction)
		{
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &APlayerControllerBase::StartJump);
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &APlayerControllerBase::StopJump);
		}

		if (DashAction)
		{
			EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Started, this, &APlayerControllerBase::StartDash);
		}

		if (InteractAction)
		{
			EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &APlayerControllerBase::StartInteract);
		}

		if (PauseAction)
		{
			EnhancedInputComponent->BindAction(PauseAction, ETriggerEvent::Started, this, &APlayerControllerBase::RequestPauseToggle);
		}
	}
}

void APlayerControllerBase::PlayRumble(float Intensity, float Duration, bool bAffectsLeftLarge, bool bAffectsLeftSmall, bool bAffectsRightLarge, bool bAffectsRightSmall)
{
	PlayDynamicForceFeedback(Intensity, Duration, bAffectsLeftLarge, bAffectsLeftSmall, bAffectsRightLarge, bAffectsRightSmall, EDynamicForceFeedbackAction::Start, FLatentActionInfo());
}

void APlayerControllerBase::PlayRumbleEffect(UForceFeedbackEffect* Effect, bool bLooping, FName Tag)
{
	if (!Effect)
	{
		return;
	}

	FForceFeedbackParameters Params;
	Params.Tag = Tag;
	Params.bLooping = bLooping;
	ClientPlayForceFeedback(Effect, Params);
}

void APlayerControllerBase::StopRumbleEffect(FName Tag)
{
	ClientStopForceFeedback(nullptr, Tag);
}

void APlayerControllerBase::NotifyItemBumped()
{
	PlayRumbleEffect(ItemBumpRumbleEffect);
}

void APlayerControllerBase::HandleKnockbackRumble(FVector SourceLocation)
{
	PlayRumbleEffect(KnockbackRumbleEffect);
}

void APlayerControllerBase::HandleDashHitRumble(ACharacter* HitCharacter)
{
	PlayRumbleEffect(DashHitRumbleEffect);
}

void APlayerControllerBase::HandleSegmentDepletedRumble(int32 SegmentIndex)
{
	PlayRumbleEffect(SegmentDepletedRumbleEffect);
}

void APlayerControllerBase::HandlePickupRumble()
{
	PlayRumbleEffect(PickupRumbleEffect);
}

void APlayerControllerBase::HandleThrowRumble()
{
	PlayRumbleEffect(ThrowRumbleEffect);
}

void APlayerControllerBase::HandleOwnDeathRumble(FVector DeathLocation)
{
	PlayRumbleEffect(DeathRumbleEffect);
}

void APlayerControllerBase::NotifyOpponentDied(FVector DeathLocation)
{
	PlayRumbleEffect(KillRumbleEffect);
}

void APlayerControllerBase::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ControlledCharacter = Cast<APlayerCharacter>(InPawn);

	if (ControlledCharacter)
	{
		if (UPushComponent* Push = ControlledCharacter->GetPushComponent())
		{
			Push->OnKnockback.AddUniqueDynamic(this, &APlayerControllerBase::HandleKnockbackRumble);
		}

		if (UDashComponent* Dash = ControlledCharacter->GetDashComponent())
		{
			Dash->OnDashHitCharacter.AddUniqueDynamic(this, &APlayerControllerBase::HandleDashHitRumble);
		}

		if (UPlayerTimerComponent* Timer = ControlledCharacter->GetTimerComponent())
		{
			Timer->OnSegmentDepleted.AddUniqueDynamic(this, &APlayerControllerBase::HandleSegmentDepletedRumble);
		}

		if (UPickupComponent* Pickup = ControlledCharacter->GetPickupComponent())
		{
			Pickup->OnItemPickedUp.AddUniqueDynamic(this, &APlayerControllerBase::HandlePickupRumble);
		}

		if (UThrowComponent* Throw = ControlledCharacter->GetThrowComponent())
		{
			Throw->OnItemThrown.AddUniqueDynamic(this, &APlayerControllerBase::HandleThrowRumble);
		}

		if (URespawnComponent* Respawn = ControlledCharacter->GetRespawnComponent())
		{
			Respawn->OnPlayerDied.AddUniqueDynamic(this, &APlayerControllerBase::HandleOwnDeathRumble);
		}
	}

	if (ControlledCharacter && TimerWidgetClass && IsLocalPlayerController())
	{
		if (!TimerWidget)
		{
			TimerWidget = CreateWidget<UPlayerTimerWidget>(this, TimerWidgetClass);
			if (TimerWidget)
			{
				TimerWidget->AddToViewport();

				int32 PlayerIndex = 0;
				if (const ULocalPlayer* LocalPlayer = GetLocalPlayer())
				{
					if (const UGameInstance* GameInstance = GetGameInstance())
					{
						const int32 FoundIndex = GameInstance->GetLocalPlayers().IndexOfByKey(LocalPlayer);
						PlayerIndex = (FoundIndex != INDEX_NONE) ? FoundIndex : 0;
					}
				}
				const bool bIsFirstPlayer = (PlayerIndex == 0);

				TimerWidget->SetAnchorsInViewport(bIsFirstPlayer ? FAnchors(0.f, 0.f) : FAnchors(1.f, 0.f));
				TimerWidget->SetAlignmentInViewport(bIsFirstPlayer ? FVector2D(0.f, 0.f) : FVector2D(1.f, 0.f));

				if (bIsFirstPlayer)
				{
					TimerWidget->SetRenderScale(FVector2D(-1.f, 1.f));
				}
			}
		}

		if (TimerWidget)
		{
			TimerWidget->InitTimerComponent(ControlledCharacter->GetTimerComponent());
		}
	}
}

void APlayerControllerBase::OnUnPossess()
{
	Super::OnUnPossess();

	ControlledCharacter = nullptr;
}

void APlayerControllerBase::Move(const FInputActionValue& Value)
{
	if (ControlledCharacter)
	{
		ControlledCharacter->Move(Value.Get<FVector2D>());
	}
}

void APlayerControllerBase::StartJump()
{
	if (ControlledCharacter)
	{
		ControlledCharacter->Jump();
	}
}

void APlayerControllerBase::StopJump()
{
	if (ControlledCharacter)
	{
		ControlledCharacter->StopJumping();
	}
}

void APlayerControllerBase::StartDash()
{
	if (ControlledCharacter)
	{
		ControlledCharacter->RequestDash();
	}
}

void APlayerControllerBase::StartInteract()
{
	if (ControlledCharacter)
	{
		ControlledCharacter->RequestInteract();
	}
}

void APlayerControllerBase::RequestPauseToggle()
{
	if (UMenuFlowSubsystem* Flow = GetGameInstance() ? GetGameInstance()->GetSubsystem<UMenuFlowSubsystem>() : nullptr)
	{
		Flow->RequestBack();
	}
}
