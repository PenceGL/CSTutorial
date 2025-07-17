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

void UInventorySubmenu::SetAmountWidgetReference(const TObjectPtr<UAmountWidget> AmountWidgetIn)
{
	AmountWidgetReference = AmountWidgetIn;
	if (AmountWidgetReference)
	{
		AmountWidgetReference->OnValueConfirm.BindUObject(this, &UInventorySubmenu::SplitConfirmed);
	}
}

void UInventorySubmenu::HideSubmenuWidgets() const
{
	if (AmountWidgetReference)
	{
		AmountWidgetReference->SetVisibility(ESlateVisibility::Collapsed);
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
}

void UInventorySubmenu::ExamineButtonClicked()
{
}

void UInventorySubmenu::DropButtonClicked()
{
	if (PlayerCharacter)
	{
		PlayerCharacter->DropItem(OriginatingItemSlot->GetItemReference());
	}
	else
	{
		UE_LOG(LogTemp, Error, L"%s: DropItem could not be completed, player character reference was nullptr!",
		       *FString(__FUNCTION__));
	}

	CloseSubmenu();
}

void UInventorySubmenu::SplitButtonClicked()
{
	if (bItemCanBeSplit && AmountWidgetReference)
	{
		bSplitInProgress = true;
		CloseSubmenu();

		AmountWidgetReference->SetHeaderText(FText::FromString("Amount to split:"));
		AmountWidgetReference->SetItemReference(OriginatingItemSlot->GetItemReference());

		float MouseX;
		float MouseY;
		if (!GetOwningPlayer()->GetMousePosition(MouseX, MouseY))
		{
			UE_LOG(LogTemp, Error, L"%s: GetOwningPlayer()->GetMousePosition() returned false!",
			       *FString(__FUNCTION__));
		}
		const FVector2D WidgetPosition{MouseX, MouseY};

		AmountWidgetReference->SetPositionInViewport(WidgetPosition);
		AmountWidgetReference->SetAlignmentInViewport(FVector2D{0.5, 0.5});
		AmountWidgetReference->SetVisibility(ESlateVisibility::Visible);
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
			PlayerCharacter->GetInventory()->SplitExistingStack(
				OriginatingItemSlot->GetItemReference(),
				AmountToSplit);
		}
		else
		{
			UE_LOG(LogTemp, Error, L"%s: Split could not be completed, player character reference was nullptr!",
			       *FString(__FUNCTION__));
		}
	}
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
