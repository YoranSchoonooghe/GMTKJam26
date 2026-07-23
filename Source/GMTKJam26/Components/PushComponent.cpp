#include "PushComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DrawDebugHelpers.h"
#include "DashComponent.h"

UPushComponent::UPushComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPushComponent::BeginPlay()
{
	Super::BeginPlay();

	OwningCharacter = Cast<ACharacter>(GetOwner());
	if (!OwningCharacter.IsValid())
	{
		return;
	}

	PushZone = NewObject<UCapsuleComponent>(OwningCharacter.Get(), TEXT("PushZone"));
	PushZone->SetCollisionProfileName(FName("OverlapAllDynamic"));
	PushZone->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Ignore);
	PushZone->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);
	PushZone->RegisterComponent();
	PushZone->AttachToComponent(OwningCharacter->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);

	RebuildZoneSize();
}

void UPushComponent::RebuildZoneSize()
{
	if (!PushZone || !OwningCharacter.IsValid())
	{
		return;
	}

	const UCapsuleComponent* OwnerCapsule = OwningCharacter->GetCapsuleComponent();
	const float Radius = (OwnerCapsule ? OwnerCapsule->GetScaledCapsuleRadius() : 34.f) + ZoneRadiusPadding;
	const float HalfHeight = (OwnerCapsule ? OwnerCapsule->GetScaledCapsuleHalfHeight() : 88.f) + ZoneHalfHeightPadding;

	PushZone->SetCapsuleSize(Radius, HalfHeight);
}

void UPushComponent::SetZoneSize(float InRadiusPadding, float InHalfHeightPadding)
{
	ZoneRadiusPadding = InRadiusPadding;
	ZoneHalfHeightPadding = InHalfHeightPadding;
	RebuildZoneSize();
}

void UPushComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	ApplyPassivePush(DeltaTime);

	if (bDrawDebugZone && PushZone)
	{
		DrawDebugCapsule(GetWorld(), PushZone->GetComponentLocation(), PushZone->GetScaledCapsuleHalfHeight(),
			PushZone->GetScaledCapsuleRadius(), PushZone->GetComponentQuat(), FColor::Green, false, -1.f, 0, 1.5f);
	}
}

void UPushComponent::ApplyPassivePush(float DeltaTime)
{
	if (!PushZone || !OwningCharacter.IsValid())
	{
		return;
	}

	UCharacterMovementComponent* MyMovement = OwningCharacter->GetCharacterMovement();
	if (!MyMovement)
	{
		return;
	}

	if (const UDashComponent* MyDash = OwningCharacter->FindComponentByClass<UDashComponent>())
	{
		if (MyDash->IsDashing())
		{
			return;
		}
	}

	const FVector MyVelocity2D(MyMovement->Velocity.X, MyMovement->Velocity.Y, 0.f);
	const float MySpeed = MyVelocity2D.Size();
	if (MySpeed < KINDA_SMALL_NUMBER)
	{
		return;
	}

	const FVector MyMoveDirection = MyVelocity2D.GetSafeNormal();

	TArray<AActor*> OverlappingActors;
	PushZone->GetOverlappingActors(OverlappingActors, ACharacter::StaticClass());

	for (AActor* Actor : OverlappingActors)
	{
		if (Actor == OwningCharacter.Get())
		{
			continue;
		}

		ACharacter* OtherCharacter = Cast<ACharacter>(Actor);
		if (!OtherCharacter)
		{
			continue;
		}

		if (const UDashComponent* OtherDash = OtherCharacter->FindComponentByClass<UDashComponent>())
		{
			if (OtherDash->IsDashing())
			{
				continue;
			}
		}

		const FVector ToOther = (OtherCharacter->GetActorLocation() - OwningCharacter->GetActorLocation()).GetSafeNormal2D();

		if (FVector::DotProduct(MyMoveDirection, ToOther) <= 0.f)
		{
			continue;
		}

		OtherCharacter->AddActorWorldOffset(ToOther * MySpeed * PassivePushSpeedFactor * DeltaTime, true);

		MyMovement->Velocity = MyVelocity2D * PassivePushSpeedFactor;
	}
}

void UPushComponent::ApplyKnockback(const FVector& SourceLocation, float Force, float UpwardForce)
{
	if (!OwningCharacter.IsValid())
	{
		return;
	}

	FVector Direction = (OwningCharacter->GetActorLocation() - SourceLocation).GetSafeNormal2D();
	if (Direction.IsNearlyZero())
	{
		Direction = OwningCharacter->GetActorForwardVector();
	}

	OwningCharacter->LaunchCharacter(Direction * Force + FVector::UpVector * UpwardForce, true, true);
}

bool UPushComponent::TryShoveNearbyPlayer()
{
	if (!OwningCharacter.IsValid())
	{
		return false;
	}

	for (ACharacter* OtherCharacter : GetOverlappingCharacters())
	{
		if (UPushComponent* OtherPush = OtherCharacter->FindComponentByClass<UPushComponent>())
		{
			OtherPush->ApplyKnockback(OwningCharacter->GetActorLocation(), ShoveForce, ShoveUpwardForce);
			return true;
		}
	}

	return false;
}

TArray<ACharacter*> UPushComponent::GetOverlappingCharacters() const
{
	TArray<ACharacter*> Result;

	if (!PushZone || !OwningCharacter.IsValid())
	{
		return Result;
	}

	TArray<AActor*> OverlappingActors;
	PushZone->GetOverlappingActors(OverlappingActors, ACharacter::StaticClass());

	for (AActor* Actor : OverlappingActors)
	{
		if (Actor == OwningCharacter.Get())
		{
			continue;
		}

		if (ACharacter* OtherCharacter = Cast<ACharacter>(Actor))
		{
			Result.Add(OtherCharacter);
		}
	}

	return Result;
}
