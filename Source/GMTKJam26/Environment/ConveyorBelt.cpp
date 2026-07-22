#include "ConveyorBelt.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Interaction/InteractableItem.h"

AConveyorBelt::AConveyorBelt()
{
	PrimaryActorTick.bCanEverTick = true;

	BeltMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BeltMesh"));
	RootComponent = BeltMesh;
	BeltMesh->SetMobility(EComponentMobility::Static);

	CarryZone = CreateDefaultSubobject<UBoxComponent>(TEXT("CarryZone"));
	CarryZone->SetupAttachment(RootComponent);
	CarryZone->SetBoxExtent(FVector(200.f, 100.f, 25.f));
	CarryZone->SetCollisionProfileName(FName("OverlapAllDynamic"));

	SpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("SpawnPoint"));
	SpawnPoint->SetupAttachment(RootComponent);
	SpawnPoint->SetRelativeLocation(FVector(-180.f, 0.f, 50.f));
}

void AConveyorBelt::BeginPlay()
{
	Super::BeginPlay();

	if (ItemClassToSpawn)
	{
		GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &AConveyorBelt::SpawnItem, SpawnInterval, true);
	}
}

void AConveyorBelt::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!CarryZone)
	{
		return;
	}

	TArray<AActor*> OverlappingActors;
	CarryZone->GetOverlappingActors(OverlappingActors);

	const FVector BeltVelocity = GetActorForwardVector() * ConveyorSpeed;

	for (AActor* Actor : OverlappingActors)
	{
		if (!Actor || Actor == this)
		{
			continue;
		}

		UPrimitiveComponent* RootPrimitive = Cast<UPrimitiveComponent>(Actor->GetRootComponent());
		if (RootPrimitive && RootPrimitive->IsSimulatingPhysics())
		{
			const FVector CurrentVelocity = RootPrimitive->GetPhysicsLinearVelocity();
			RootPrimitive->SetPhysicsLinearVelocity(FVector(BeltVelocity.X, BeltVelocity.Y, CurrentVelocity.Z));
		}
		else
		{
			Actor->AddActorWorldOffset(BeltVelocity * DeltaTime, true);
		}
	}
}

void AConveyorBelt::SpawnItem()
{
	if (!ItemClassToSpawn || !SpawnPoint)
	{
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	GetWorld()->SpawnActor<AInteractableItem>(ItemClassToSpawn, SpawnPoint->GetComponentLocation(), SpawnPoint->GetComponentRotation(), SpawnParams);
}
