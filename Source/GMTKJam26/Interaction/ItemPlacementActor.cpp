#include "ItemPlacementActor.h"
#include "Components/BoxComponent.h"
#include "InteractableItem.h"

AItemPlacementActor::AItemPlacementActor()
{
	PrimaryActorTick.bCanEverTick = true;

	auto* pRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = pRoot;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnVolume"));
	TriggerBox->SetupAttachment(pRoot);

	PreviewMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PreviewMesh"));
	PreviewMesh->SetupAttachment(pRoot);
	PreviewMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AItemPlacementActor::OnBeginOverlap);
}

void AItemPlacementActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AItemPlacementActor::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (_bHasItem) return;

	auto* pItem = Cast<AInteractableItem>(OtherActor);
	if (!pItem) return;

	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Item entered trigger");

	FTimerHandle TimerHandle;

	GetWorld()->GetTimerManager().SetTimer(TimerHandle,
		[this, pItem]()
		{
			if (!IsValid(this) || !IsValid(pItem))
				return;

			pItem->SnapToAttachPoint(GetActorLocation(), GetActorRotation());
			_bHasItem = true;
			PreviewMesh->SetHiddenInGame(true);
		},
		0.05f,
		false
	);
}

void AItemPlacementActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

