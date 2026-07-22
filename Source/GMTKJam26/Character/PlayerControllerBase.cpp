#include "PlayerControllerBase.h"
#include "PlayerCharacter.h"
#include "Camera/TopDownCameraActor.h"
#include "UI/PlayerTimerWidget.h"
#include "Components/PlayerTimerComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"

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
	}
}

void APlayerControllerBase::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ControlledCharacter = Cast<APlayerCharacter>(InPawn);

	if (AActor* StaticCamera = UGameplayStatics::GetActorOfClass(GetWorld(), ATopDownCameraActor::StaticClass()))
	{
		SetViewTarget(StaticCamera);
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
