#include "InteractableItem.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"
#include "Components/PushComponent.h"
#include "Components/BlobShadowComponent.h"
#include "GMTKJam26/Character/PlayerControllerBase.h"
#include "GMTKJam26/Events/EventBusSubsystem.h"

AInteractableItem::AInteractableItem()
{
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	Mesh->SetSimulatePhysics(true);
	Mesh->SetCollisionProfileName(FName("PhysicsActor"));
	Mesh->SetNotifyRigidBodyCollision(true);

	BlobShadowComponent = CreateDefaultSubobject<UBlobShadowComponent>(TEXT("BlobShadowComponent"));
}

void AInteractableItem::BeginPlay()
{
	Super::BeginPlay();

	Mesh->OnComponentHit.AddDynamic(this, &AInteractableItem::OnMeshHit);
}

bool AInteractableItem::CanBePickedUp() const
{
	if (State == EInteractableItemState::Attached)
		return false;

	return bCanBePickedUp && State != EInteractableItemState::Held;
}

void AInteractableItem::OnPickedUp(USceneComponent* AttachTo, FName SocketName)
{
	if (!bCanBePickedUp || !AttachTo)
	{
		return;
	}

	GetWorldTimerManager().ClearTimer(PickupCooldownTimerHandle);

	Mesh->SetSimulatePhysics(false);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AttachToComponent(AttachTo, FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);

	LastHolder = AttachTo->GetOwner();
	State = EInteractableItemState::Held;
}

void AInteractableItem::Release()
{
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetSimulatePhysics(true);

	State = EInteractableItemState::Flying;

	bCanBePickedUp = false;
	GetWorldTimerManager().SetTimer(PickupCooldownTimerHandle, this, &AInteractableItem::ResetPickupCooldown, PickupCooldown, false);
}

void AInteractableItem::Throw(const FVector& Velocity)
{
	Release();

	Mesh->AddImpulse(Velocity, NAME_None, true);

	OnThrown.Broadcast();
}

void AInteractableItem::SnapToAttachPoint(const FVector& Location, const FRotator& Rotation)
{
	State = EInteractableItemState::Attached;

	Mesh->SetPhysicsLinearVelocity(FVector::ZeroVector);
	Mesh->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
	Mesh->SetSimulatePhysics(false);

	bCanBePickedUp = false;

	_startLocation = GetActorLocation();
	_startRotation = GetActorRotation();

	_targetLocation = Location;
	_targetRotation = Rotation;

	_elapsedAttachTime = 0.0f;
	_bIsAttaching = true;

	//OnAttached.Broadcast(Location);
}

void AInteractableItem::Explode(const FVector& ImpulseDirection, float ImpulseStrength)
{
	_bIsAttaching = false;

	State = EInteractableItemState::Flying;
	bCanBePickedUp = false;

	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetSimulatePhysics(true);
	Mesh->AddImpulse(ImpulseDirection.GetSafeNormal() * ImpulseStrength, NAME_None, true);
}

void AInteractableItem::SetHighlight(bool highlighted)
{
	if (highlighted)
	{
		Mesh->SetOverlayMaterial(OutlineMaterial);
	}
	else
	{
		Mesh->SetOverlayMaterial(nullptr);
	}
}

void AInteractableItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateAttachedState(DeltaTime);
}

void AInteractableItem::OnMeshHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (ACharacter* HitCharacter = Cast<ACharacter>(OtherActor))
	{
		if (State == EInteractableItemState::Flying)
		{
			if (HitCharacter != LastHolder.Get())
			{
				if (UPushComponent* HitPush = HitCharacter->FindComponentByClass<UPushComponent>())
				{
					HitPush->ApplyKnockback(Hit.ImpactPoint, ThrownHitPushForce, ThrownHitPushUpwardForce);
				}

				OnHitCharacter.Broadcast(HitCharacter);
			}
		}
		else if (State == EInteractableItemState::Grounded)
		{
			if (APlayerControllerBase* PC = Cast<APlayerControllerBase>(HitCharacter->GetController()))
			{
				PC->NotifyItemBumped();
			}

			OnBumpedByCharacter.Broadcast(HitCharacter);
		}

		return;
	}

	if (State != EInteractableItemState::Flying)
	{
		return;
	}

	State = EInteractableItemState::Grounded;
	OnLanded.Broadcast(Hit.ImpactPoint);
}

void AInteractableItem::ResetPickupCooldown()
{
	bCanBePickedUp = true;
}

void AInteractableItem::UpdateAttachedState(float DeltaTime)
{
	if (!_bIsAttaching) return;

	_elapsedAttachTime += DeltaTime;

	float alpha = FMath::Clamp(_elapsedAttachTime / AttachDuration, 0.0f, 1.0f);

	alpha = FMath::InterpEaseInOut(0.0f, 1.0f, alpha, 2.0f);

	SetActorLocation(FMath::Lerp(_startLocation, _targetLocation, alpha));
	SetActorRotation(FMath::Lerp(_startRotation, _targetRotation, alpha));

	if (alpha >= 1.0f)
	{
		_bIsAttaching = false;

		SetActorLocation(_targetLocation);
		SetActorRotation(_targetRotation);

		OnAttached.Broadcast(_targetLocation);
		GetWorld()->GetSubsystem<UEventBusSubsystem>()->SendEvent("OnRobotPartAttached");
	}
}
