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

	if (bSpawningEnabled && ItemsToSpawn.Num() > 0)
	{
		ScheduleNextSpawn();
	}
}

void AConveyorBelt::EnableSpawning()
{
	if (bSpawningEnabled)
	{
		return;
	}

	bSpawningEnabled = true;

	if (ItemsToSpawn.Num() > 0 && !GetWorldTimerManager().IsTimerActive(SpawnTimerHandle))
	{
		ScheduleNextSpawn();
	}
}

void AConveyorBelt::DisableSpawning()
{
	bSpawningEnabled = false;
	GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
}

void AConveyorBelt::ScheduleNextSpawn()
{
	const float Interval = FMath::FRandRange(FMath::Max(MinSpawnInterval, 0.f), FMath::Max(MaxSpawnInterval, MinSpawnInterval));
	GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &AConveyorBelt::SpawnItem, Interval, false);
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
			const FVector CurrentXY(CurrentVelocity.X, CurrentVelocity.Y, 0.f);
			const FVector TargetXY(BeltVelocity.X, BeltVelocity.Y, 0.f);

			const FVector NewXY = FMath::VInterpConstantTo(CurrentXY, TargetXY, DeltaTime, ConveyorAcceleration);
			RootPrimitive->SetPhysicsLinearVelocity(FVector(NewXY.X, NewXY.Y, CurrentVelocity.Z));
		}
		else
		{
			Actor->AddActorWorldOffset(BeltVelocity * DeltaTime, true);
		}
	}
}

void AConveyorBelt::SpawnItem()
{
	TSubclassOf<AInteractableItem> ChosenClass = PickRandomItemClass();
	if (ChosenClass && SpawnPoint)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		GetWorld()->SpawnActor<AInteractableItem>(ChosenClass, SpawnPoint->GetComponentLocation(), SpawnPoint->GetComponentRotation(), SpawnParams);
	}

	if (bSpawningEnabled && ItemsToSpawn.Num() > 0)
	{
		ScheduleNextSpawn();
	}
}

TSubclassOf<AInteractableItem> AConveyorBelt::PickRandomItemClass() const
{
	float TotalWeight = 0.f;
	for (const FConveyorSpawnEntry& Entry : ItemsToSpawn)
	{
		if (Entry.ItemClass)
		{
			TotalWeight += FMath::Max(Entry.SpawnWeight, 0.f);
		}
	}

	if (TotalWeight <= 0.f)
	{
		return nullptr;
	}

	float Roll = FMath::FRandRange(0.f, TotalWeight);
	for (const FConveyorSpawnEntry& Entry : ItemsToSpawn)
	{
		if (!Entry.ItemClass)
		{
			continue;
		}

		Roll -= FMath::Max(Entry.SpawnWeight, 0.f);
		if (Roll <= 0.f)
		{
			return Entry.ItemClass;
		}
	}

	return ItemsToSpawn.Last().ItemClass;
}
