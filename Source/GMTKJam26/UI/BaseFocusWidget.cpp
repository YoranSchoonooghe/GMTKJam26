#include "BaseFocusWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Framework/Application/SlateApplication.h"
#include "GameFramework/PlayerController.h"

void UBaseFocusWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!FirstFocusedElement && WidgetTree)
	{
		WidgetTree->ForEachWidget([this](UWidget* Widget)
		{
			if (!FirstFocusedElement)
			{
				if (UButton* Btn = Cast<UButton>(Widget))
				{
					FirstFocusedElement = Btn;
				}
			}
		});
	}

	if (APlayerController* PC = GetOwningPlayer())
	{
		FInputModeUIOnly InputMode;

		if (FirstFocusedElement)
		{
			InputMode.SetWidgetToFocus(FirstFocusedElement->TakeWidget());
		}

		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

		PC->SetInputMode(InputMode);
		PC->SetShowMouseCursor(true);
	}

	FocusButton(FirstFocusedElement);
}

void UBaseFocusWidget::NativeDestruct()
{
	if (APlayerController* PC = GetOwningPlayer())
	{
		PC->SetInputMode(FInputModeGameOnly());
		PC->SetShowMouseCursor(false);
	}

	Super::NativeDestruct();
}

void UBaseFocusWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!FirstFocusedElement || !WidgetTree)
	{
		return;
	}

	const TSharedPtr<SWidget> FocusedSlate = FSlateApplication::Get().GetUserFocusedWidget(0);

	UButton* FocusedButton = nullptr;
	if (FocusedSlate.IsValid())
	{
		WidgetTree->ForEachWidget([&FocusedButton, &FocusedSlate](UWidget* Widget)
		{
			if (!FocusedButton)
			{
				if (UButton* Btn = Cast<UButton>(Widget))
				{
					if (Btn->GetCachedWidget() == FocusedSlate)
					{
						FocusedButton = Btn;
					}
				}
			}
		});
	}

	if (FocusedButton)
	{
		LastFocusedButton = FocusedButton;
		return;
	}

	FocusButton(LastFocusedButton.IsValid() ? LastFocusedButton.Get() : FirstFocusedElement.Get());
}

void UBaseFocusWidget::FocusButton(UButton* Button) const
{
	if (Button)
	{
		Button->SetKeyboardFocus();
	}
}
