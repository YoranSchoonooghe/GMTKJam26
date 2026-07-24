#include "ItemPlacementActor.h"
#include "Components/BoxComponent.h"
#include "InteractableItem.h"
#include "GMTKJam26/Character/PlayerCharacter.h"
#include "GMTKJam26/Components/PickupComponent.h"
#include "GMTKJam26/Components/ThrowComponent.h"
#include "GMTKJam26/Components/DropComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/WidgetComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "RobotPart.h"
#include "Character/PlayerCharacter.h"
#include "Character/PlayerControllerBase.h"
#include "Components/PlayerTimerComponent.h"

namespace
{
	void CopyMaterialParameters(UMaterialInterface* Source, UMaterialInstanceDynamic* Target)
	{
		if (!Source || !Target)
		{
			return;
		}

		TArray<FMaterialParameterInfo> ParameterInfos;
		TArray<FGuid> ParameterIds;

		Source->GetAllVectorParameterInfo(ParameterInfos, ParameterIds);
		for (const FMaterialParameterInfo& Info : ParameterInfos)
		{
			FLinearColor Value;
			if (Source->GetVectorParameterValue(Info, Value))
			{
				Target->SetVectorParameterValueByInfo(Info, Value);
			}
		}

		ParameterInfos.Reset();
		ParameterIds.Reset();
		Source->GetAllScalarParameterInfo(ParameterInfos, ParameterIds);
		for (const FMaterialParameterInfo& Info : ParameterInfos)
		{
			float Value = 0.f;
			if (Source->GetScalarParameterValue(Info, Value))
			{
				Target->SetScalarParameterValueByInfo(Info, Value);
			}
		}

		ParameterInfos.Reset();
		ParameterIds.Reset();
		Source->GetAllTextureParameterInfo(ParameterInfos, ParameterIds);
		for (const FMaterialParameterInfo& Info : ParameterInfos)
		{
			UTexture* Value = nullptr;
			if (Source->GetTextureParameterValue(Info, Value))
			{
				Target->SetTextureParameterValueByInfo(Info, Value);
			}
		}
	}
}

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

void AItemPlacementActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	RefreshPreviewFromExpectedItem();
}

void AItemPlacementActor::RefreshPreviewFromExpectedItem()
{
	if (!PreviewMesh || !ExpectedItemClass)
	{
		return;
	}

	const ARobotPart* DefaultItem = GetDefault<ARobotPart>(ExpectedItemClass);
	if (!DefaultItem)
	{
		return;
	}

	ExpectedPartType = DefaultItem->GetPartType();

	UStaticMeshComponent* DefaultMesh = DefaultItem->GetItemMesh();
	if (!DefaultMesh)
	{
		return;
	}

	PreviewMesh->SetStaticMesh(DefaultMesh->GetStaticMesh());
	PreviewMesh->SetRelativeTransform(DefaultMesh->GetRelativeTransform());

	if (GhostMaterial)
	{
		const int32 NumMaterials = DefaultMesh->GetNumMaterials();
		for (int32 Index = 0; Index < NumMaterials; ++Index)
		{
			UMaterialInstanceDynamic* GhostInstance = UMaterialInstanceDynamic::Create(GhostMaterial, this);
			CopyMaterialParameters(DefaultMesh->GetMaterial(Index), GhostInstance);
			GhostInstance->SetScalarParameterValue(GhostOpacityParameterName, GhostOpacity);

			PreviewMesh->SetMaterial(Index, GhostInstance);
		}
	}
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
		if (auto* pDropComponent = _targetPlayer->GetDropComponent())
		{
			pDropComponent->OnItemLost.AddDynamic(this, &AItemPlacementActor::ClearTargetItem);
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

	if (!_targetItem) return;
	if (!_bPlayerIsInRange || _targetItem->GetPartType() != ExpectedPartType)
	{
		_targetItem = nullptr;
		return;
	}

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

			if (APlayerControllerBase* PC = Cast<APlayerControllerBase>(HolderCharacter->GetController()))
			{
				PC->NotifyItemPlaced();
			}
		}

		OnItemPlaced.Broadcast(RobotPart);
	}
}

void AItemPlacementActor::ClearTargetItem()
{
	PreviewMesh->SetHiddenInGame(true);
	InteractWidget->SetHiddenInGame(true);

	_targetItem = nullptr;
}

