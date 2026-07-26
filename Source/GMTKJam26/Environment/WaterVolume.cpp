#include "WaterVolume.h"
#include "Components/BoxComponent.h"

AWaterVolume::AWaterVolume()
{
	PrimaryActorTick.bCanEverTick = false;

	WaterZone = CreateDefaultSubobject<UBoxComponent>(TEXT("WaterZone"));
	RootComponent = WaterZone;

	WaterZone->SetBoxExtent(FVector(500.f, 500.f, 50.f));
	WaterZone->SetCollisionProfileName(FName("OverlapAllDynamic"));
}

void AWaterVolume::BeginPlay()
{
	Super::BeginPlay();

	WaterZone->OnComponentBeginOverlap.AddDynamic(this, &AWaterVolume::OnZoneOverlap);
}

void AWaterVolume::OnZoneOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == this)
	{
		return;
	}

	const float VerticalVelocity = OtherActor->GetVelocity().Z;
	if (VerticalVelocity > -MinDownwardSpeed)
	{
		return;
	}

	const FVector SplashLocation = bFromSweep ? FVector(SweepResult.Location) : OtherActor->GetActorLocation();

	OnWaterSplash.Broadcast(OtherActor, SplashLocation, -VerticalVelocity);
}
