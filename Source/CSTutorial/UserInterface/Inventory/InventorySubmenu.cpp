//game
#include "UserInterface/Inventory/InventorySubMenu.h"
#include "UserInterface/Inventory/InventoryItemSlot.h"
#include "UserInterface/Inventory/AmountWidget.h"
#include "Components/InventoryComponent.h"
#include "Player/CSTutorialCharacter.h"
#include "Items/ItemBase.h"

// engine
#include "Components/Button.h"

void UInventorySubmenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (AmountWidgetClass)
	{
		AmountWidget = CreateWidget<UAmountWidget>(this, AmountWidgetClass);
		AmountWidget->AddToViewport(7);
		AmountWidget->SetVisibility(ESlateVisibility::Collapsed);

		AmountWidget->OnValueConfirm.BindUObject(this, &UInventorySubmenu::SplitConfirmed);
	}
	else
	{
		UE_LOG(LogTemp, Error, L"%s: AmountWidgetClass was null!", *FString(__FUNCTION__));
	}

	UseButton->OnClicked.AddDynamic(this, &UInventorySubmenu::UseButtonClicked);
	ExamineButton->OnClicked.AddDynamic(this, &UInventorySubmenu::ExamineButtonClicked);
	DropButton->OnClicked.AddDynamic(this, &UInventorySubmenu::DropButtonClicked);
	SplitButton->OnClicked.AddDynamic(this, &UInventorySubmenu::SplitButtonClicked);

	bSubMenuActive = false;
	bSplitInProgress = false;

	
}

void UInventorySubmenu::NativeConstruct()
{
	Super::NativeConstruct();
}

void UInventorySubmenu::HideSubmenuWidgets() const
{
	if (AmountWidget)
	{
		AmountWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UInventorySubmenu::ConfigureSubmenuButtons()
{
	if (OriginatingItemSlot->GetItemReference()->NumericData.bIsStackable &&
		OriginatingItemSlot->GetItemReference()->Quantity > 1)
	{
		SplitButton->SetRenderOpacity(1.0);
		// SplitButton->SetIsEnabled(true);
		bItemCanBeSplit = true;
	}
	else
	{
		// TODO: Find a better way to visually show the button is disabled
		SplitButton->SetRenderOpacity(0.6);
		// SplitButton->SetIsEnabled(false);
		bItemCanBeSplit = false;
	}
}

void UInventorySubmenu::UseButtonClicked()
{
	// TODO
}

void UInventorySubmenu::ExamineButtonClicked()
{
	// TODO
}

void UInventorySubmenu::DropButtonClicked()
{
	if (PlayerCharacter)
	{
		UE_LOG(LogTemp, Error, L"%s: Calling DropItem with %s", *FString(__FUNCTION__), *OriginatingItemSlot->GetItemReference()->TextData.Name.ToString());
		PlayerCharacter->DropItem(OriginatingItemSlot->GetItemReference());
	}
	else
	{
		UE_LOG(LogTemp, Error, L"%s: DropItem could not be completed, player character reference was nullptr!", *FString(__FUNCTION__));
	}

	CloseSubmenu();
}

void UInventorySubmenu::SplitButtonClicked()
{
	if (bItemCanBeSplit && AmountWidget)
	{
		bSplitInProgress = true;
		CloseSubmenu();

		AmountWidget->SetHeaderText(FText::FromString("Amount to split:"));
		AmountWidget->SetItemReference(OriginatingItemSlot->GetItemReference());

		float MouseX;
		float MouseY;
		if (!GetOwningPlayer()->GetMousePosition(MouseX, MouseY))
		{
			UE_LOG(LogTemp, Error, L"%s: GetOwningPlayer()->GetMousePosition() returned false!",
			       *FString(__FUNCTION__));
		}
		const FVector2D WidgetPosition{MouseX, MouseY};

		AmountWidget->SetPositionInViewport(WidgetPosition);
		AmountWidget->SetAlignmentInViewport(FVector2D{0.5, 0.5});
		AmountWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void UInventorySubmenu::SplitConfirmed(const int32 AmountToSplit)
{
	if (AmountToSplit >= OriginatingItemSlot->GetItemReference()->Quantity || AmountToSplit == 0)
	{
		UE_LOG(LogTemp, Warning, L"%s: No split performed. Split amount was equal to item quantity or 0.",
		       *FString(__FUNCTION__));
	}
	else
	{
		if (PlayerCharacter)
		{
			PlayerCharacter->GetInventory()->SplitExistingStack(OriginatingItemSlot->GetItemReference(), AmountToSplit);
		}
		else
		{
			UE_LOG(LogTemp, Error, L"%s: Split could not be completed, player character reference was nullptr!",
			       *FString(__FUNCTION__));
		}
	}
	bSplitInProgress = false;
}

void UInventorySubmenu::CloseSubmenu()
{
	// do not null OriginatingItemSlot when closing the submenu due to a split
	if (bSplitInProgress)
	{
		bSubMenuActive = false;
		SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		OriginatingItemSlot = nullptr;
		bSplitInProgress = false;
		bSubMenuActive = false;
		SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UInventorySubmenu::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	CloseSubmenu();
}
