#include "ItemPlacementActor.h"
#include "Components/BoxComponent.h"
#include "InteractableItem.h"
#include "GMTKJam26/Character/PlayerCharacter.h"
#include "GMTKJam26/Components/PickupComponent.h"
#include "GMTKJam26/Components/ThrowComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/WidgetComponent.h"
#include "RobotPart.h"
#include "Character/PlayerCharacter.h"
#include "Components/PlayerTimerComponent.h"

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

	InteractWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractWidget"));
	InteractWidget->SetupAttachment(pRoot);
	InteractWidget->SetHiddenInGame(true);

	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AItemPlacementActor::OnBeginOverlap);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &AItemPlacementActor::OnEndOverlap);
}

void AItemPlacementActor::BeginPlay()
{
	Super::BeginPlay();
	
	PreviewMesh->SetHiddenInGame(true);

	_targetPlayer = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(this, PlayerIndex));
	if (_targetPlayer)
	{
		if (auto* pPickupComponent = _targetPlayer->GetPickupComponent())
		{
			pPickupComponent->OnItemPickedUp.AddDynamic(this, &AItemPlacementActor::RetrievePlayerItem);
			pPickupComponent->OnItemDropped.AddDynamic(this, &AItemPlacementActor::TryAttach);
		}
		if (auto* pThrowComponent = _targetPlayer->GetThrowComponent())
		{
			pThrowComponent->OnItemThrown.AddDynamic(this, &AItemPlacementActor::TryAttach);
		}
	}
}

void AItemPlacementActor::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (_bHasItem) return;

	if (auto* pPlayer = Cast<APlayerCharacter>(OtherActor))
	{
		if (_targetPlayer != pPlayer) return;

		_bPlayerIsInRange = true;

		if (!_targetItem) return;

		if (_targetItem->GetPartType() == ExpectedPartType)
		{
			InteractWidget->SetHiddenInGame(false);
		}
	}
}

void AItemPlacementActor::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Cast<APlayerCharacter>(OtherActor))
	{
		_bPlayerIsInRange = false;
		InteractWidget->SetHiddenInGame(true);
	}
}

void AItemPlacementActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AItemPlacementActor::RetrievePlayerItem()
{
	if (_bHasItem) return;

	_targetItem = Cast<ARobotPart>(_targetPlayer->GetPickupComponent()->GetHeldItem());
	if (!_targetItem) return;

	if (_targetItem->GetPartType() == ExpectedPartType)
	{
		PreviewMesh->SetHiddenInGame(false);
	}
}

void AItemPlacementActor::TryAttach()
{
	PreviewMesh->SetHiddenInGame(true);
	InteractWidget->SetHiddenInGame(true);

	if (!_bPlayerIsInRange) return;
	if (!_targetItem) return;
	if (_targetItem->GetPartType() != ExpectedPartType) return;

	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Item entered trigger");

	_targetItem->SnapToAttachPoint(PreviewMesh->GetComponentLocation(), PreviewMesh->GetComponentRotation());
	_bHasItem = true;
	PreviewMesh->SetHiddenInGame(true);

	if (ARobotPart* RobotPart = Cast<ARobotPart>(_targetItem))
	{
		if (APlayerCharacter* HolderCharacter = Cast<APlayerCharacter>(RobotPart->GetLastHolder()))
		{
			if (UPlayerTimerComponent* TimerComponent = HolderCharacter->GetTimerComponent())
			{
				TimerComponent->AddItem(RobotPart);
			}
		}
	}
}

