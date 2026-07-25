#include "MenuButtonBase.h"

UMenuButtonBase::UMenuButtonBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	OnHovered.AddDynamic(this, &UMenuButtonBase::HandleHovered);
	OnUnhovered.AddDynamic(this, &UMenuButtonBase::HandleUnhovered);
	OnReceivedFocus.BindUObject(this, &UMenuButtonBase::HandleReceivedFocus);
	OnLostFocus.BindUObject(this, &UMenuButtonBase::HandleLostFocus);
}

void UMenuButtonBase::HandleHovered()
{
	bIsMouseHovered = true;
	RefreshBumpState();
}

void UMenuButtonBase::HandleUnhovered()
{
	bIsMouseHovered = false;
	RefreshBumpState();
}

void UMenuButtonBase::HandleReceivedFocus()
{
	bIsFocused = true;
	RefreshBumpState();
}

void UMenuButtonBase::HandleLostFocus()
{
	bIsFocused = false;
	RefreshBumpState();
}

void UMenuButtonBase::RefreshBumpState()
{
	const bool bBumped = bIsMouseHovered || bIsFocused;

	SetRenderTransformPivot(FVector2D(0.5f, 0.5f));
	SetRenderScale(bBumped ? FVector2D(BumpScale, BumpScale) : FVector2D(1.f, 1.f));
	SetRenderTransformAngle(bBumped ? BumpAngle : 0.f);
}
