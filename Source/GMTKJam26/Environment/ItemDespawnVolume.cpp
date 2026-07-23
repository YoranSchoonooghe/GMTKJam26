#include "ItemDespawnVolume.h"
#include "Components/BoxComponent.h"
#include "Interaction/InteractableItem.h"

AItemDespawnVolume::AItemDespawnVolume()
{
	PrimaryActorTick.bCanEverTick = false;

	DespawnZone = CreateDefaultSubobject<UBoxComponent>(TEXT("DespawnZone"));
	RootComponent = DespawnZone;

	DespawnZone->SetBoxExtent(FVector(500.f, 500.f, 500.f));
	DespawnZone->SetCollisionProfileName(FName("OverlapAllDynamic"));
}

void AItemDespawnVolume::BeginPlay()
{
	Super::BeginPlay();

	DespawnZone->OnComponentBeginOverlap.AddDynamic(this, &AItemDespawnVolume::OnZoneOverlap);
}

void AItemDespawnVolume::OnZoneOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AInteractableItem* Item = Cast<AInteractableItem>(OtherActor))
	{
		Item->Destroy();
	}
}
