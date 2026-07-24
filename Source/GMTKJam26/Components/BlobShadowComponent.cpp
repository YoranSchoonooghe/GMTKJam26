#include "BlobShadowComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/DecalComponent.h"
#include "Kismet/KismetMathLibrary.h"

UBlobShadowComponent::UBlobShadowComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickGroup = TG_PostPhysics;
}


void UBlobShadowComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* Owner = GetOwner();
	if (!Owner || !ShadowMaterial)
	{
		SetComponentTickEnabled(false);
		return;
	}

	DynamicMaterial = UMaterialInstanceDynamic::Create(ShadowMaterial, this);

	DecalComponent = NewObject<UDecalComponent>(Owner, TEXT("BlobShadowDecal"));
	if (DecalComponent)
	{
		DecalComponent->RegisterComponent();
		DecalComponent->SetDecalMaterial(DynamicMaterial);
		DecalComponent->DecalSize = DecalSize;

		DecalComponent->AttachToComponent(Owner->GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		DecalComponent->SetAbsolute(true, true, true);
	}
}

void UBlobShadowComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (DecalComponent)
	{
		DecalComponent->DestroyComponent();
	}

	Super::EndPlay(EndPlayReason);
}


void UBlobShadowComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateShadow();
}

void UBlobShadowComponent::UpdateShadow()
{
	auto* pOwner = GetOwner();
	if (!pOwner || !DecalComponent) return;

	const FVector start = pOwner->GetActorLocation();
	const FVector end = start - FVector(0.0f, 0.0f, MaxTraceDistance);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(pOwner);

	FHitResult HitResult;
	const bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, start, end, TraceChannel, QueryParams);

	if (bHit)
	{
		if (!DecalComponent->IsVisible())
		{
			DecalComponent->SetVisibility(true);
		}

		const FVector DecalLocation = HitResult.ImpactPoint + (HitResult.ImpactNormal * 1.0f);

		const FRotator DecalRotation = UKismetMathLibrary::MakeRotFromX(-HitResult.ImpactNormal);

		DecalComponent->SetWorldLocationAndRotation(DecalLocation, DecalRotation);

		const float distance = HitResult.Distance;
		const float relativeDistance = FMath::Clamp(distance / MaxTraceDistance, 0.0f, 1.0f);

		const float opacity = 1.0f - relativeDistance;
		if (DynamicMaterial)
		{
			DynamicMaterial->SetScalarParameterValue(OpacityParamName, opacity);
		}

		const float scale = FMath::Lerp(1.0f, 0.4f, relativeDistance);
		DecalComponent->DecalSize = DecalSize * scale;
	}
	else
	{
		if (DecalComponent->IsVisible())
		{
			DecalComponent->SetVisibility(false);
		}
	}
}

