#include "InteractableItem.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"
#include "Components/PushComponent.h"

AInteractableItem::AInteractableItem()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	Mesh->SetSimulatePhysics(true);
	Mesh->SetCollisionProfileName(FName("PhysicsActor"));
	Mesh->SetNotifyRigidBodyCollision(true);
}

void AInteractableItem::BeginPlay()
{
	Super::BeginPlay();

	Mesh->OnComponentHit.AddDynamic(this, &AInteractableItem::OnMeshHit);
}

bool AInteractableItem::CanBePickedUp() const
{
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

	SetActorLocation(Location);
	SetActorRotation(Rotation);

	bCanBePickedUp = false;

	OnAttached.Broadcast(Location);
}

void AInteractableItem::OnMeshHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (State != EInteractableItemState::Flying)
	{
		return;
	}

	if (ACharacter* HitCharacter = Cast<ACharacter>(OtherActor))
	{
		if (UPushComponent* HitPush = HitCharacter->FindComponentByClass<UPushComponent>())
		{
			HitPush->ApplyKnockback(GetActorLocation(), ThrownHitPushForce, ThrownHitPushUpwardForce);
		}

		OnHitCharacter.Broadcast(HitCharacter);
		return;
	}

	State = EInteractableItemState::Grounded;
	OnLanded.Broadcast(Hit.ImpactPoint);
}

void AInteractableItem::ResetPickupCooldown()
{
	bCanBePickedUp = true;
}
