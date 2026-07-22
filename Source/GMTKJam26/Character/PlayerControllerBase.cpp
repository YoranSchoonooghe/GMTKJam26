#include "PlayerControllerBase.h"
#include "PlayerCharacter.h"
#include "Camera/TopDownCameraActor.h"
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
