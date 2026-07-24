#include "FallingStair.h"
#include "GMTKJam26/Events/EventBusSubsystem.h"

AFallingStair::AFallingStair()
{
	PrimaryActorTick.bCanEverTick = true;

	StairMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StairMesh"));
	StairMesh->SetupAttachment(GetRootComponent());
}

void AFallingStair::BeginPlay()
{
	Super::BeginPlay();
	
	_startLocation = GetActorLocation();
	_phaseOffset = FMath::FRandRange(0.0f, 2.0f * PI);

	auto* pEventBus = GetWorld()->GetSubsystem<UEventBusSubsystem>();
	pEventBus->OnPlayerDied.AddDynamic(this, &AFallingStair::Reset);
}

void AFallingStair::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch (_state)
	{
	case EStairState::Idle:
		UpdateIdleState(DeltaTime);
		break;
	case EStairState::Shaking:
		UpdateShakingState(DeltaTime);
		break;
	case EStairState::Falling:
		UpdateFallingState(DeltaTime);
		break;
	case EStairState::Cooldown:
		break;
	case EStairState::Resetting:
		UpdateResettingState(DeltaTime);
		break;
	}
}

void AFallingStair::Reset()
{
	if (_state != EStairState::Cooldown && _state != EStairState::Falling) return;

	_cooldownLocation = GetActorLocation();

	SetActorHiddenInGame(false);
	_state = EStairState::Resetting;
	_elapsedTimeInState = 0.0f;
}

void AFallingStair::UpdateIdleState(float DeltaTime)
{
	_elapsedTimeInState += DeltaTime;

	if (_elapsedTimeInState >= TimeToFall)
	{
		_elapsedTimeInState = 0.0f;
		_state = EStairState::Shaking;

		return;
	}

	FVector location = _startLocation;
	location.Z += FMath::Sin((_elapsedTimeInState + _phaseOffset) * FloatFrequency * 2.0f * PI) * FloatAmplitude;

	SetActorLocation(location);
}

void AFallingStair::UpdateShakingState(float DeltaTime)
{
	_elapsedTimeInState += DeltaTime;

	if (_elapsedTimeInState >= ShakeDuration)
	{
		_elapsedTimeInState = 0.0f;
		_state = EStairState::Falling;

		SetActorLocation(_startLocation);

		return;
	}

	FVector shakeOffset = FVector(
		FMath::RandRange(-ShakeIntensity, ShakeIntensity),
		FMath::RandRange(-ShakeIntensity, ShakeIntensity),
		0.0f
	);

	SetActorLocation(_startLocation + shakeOffset);
}

void AFallingStair::UpdateFallingState(float DeltaTime)
{
	_elapsedTimeInState += DeltaTime;

	const float fallSpeed = GravityMultiplier * GetWorld()->GetGravityZ() * _elapsedTimeInState;

	FVector location = GetActorLocation() + FVector(0.0f, 0.0f, fallSpeed * _elapsedTimeInState);

	if (location.Z < (_startLocation.Z - FallDistance))
	{
		_elapsedTimeInState = 0.0f;
		_state = EStairState::Cooldown;

		SetActorHiddenInGame(true);

		SetActorLocation(FVector(
			_startLocation.X,
			_startLocation.Y,
			_startLocation.Z - FallDistance
		));

		return;
	}

	SetActorLocation(location);
}

void AFallingStair::UpdateResettingState(float DeltaTime)
{
	_elapsedTimeInState += DeltaTime;

	float alpha = FMath::Clamp(_elapsedTimeInState / ResetDuration, 0.0f, 1.0f);

	alpha = FMath::InterpEaseInOut(0.0f, 1.0f, alpha, 2.0f);

	SetActorLocation(FMath::Lerp(_cooldownLocation, _startLocation, alpha));

	if (alpha >= 1.0f)
	{
		_elapsedTimeInState = 0.0f;
		_state = EStairState::Idle;
		
		SetActorLocation(_startLocation);
	}
}


