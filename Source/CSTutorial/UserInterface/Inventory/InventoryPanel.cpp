// game
#include "UserInterface/Inventory/InventoryPanel.h"
#include "Player/CSTutorialCharacter.h"
#include "Components/InventoryComponent.h"
#include "UserInterface/Inventory/InventoryItemSlot.h"
#include "UserInterface/Inventory/ItemDragDropOperation.h"
#include "UserInterface/Inventory/InventorySubmenu.h"

// engine
#include "Components/TextBlock.h"
#include "Components/WrapBox.h"

void UInventoryPanel::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	bIsLinkedToInventory = false;
}

void UInventoryPanel::LinkToInventory(const TObjectPtr<UInventoryComponent>& InputInventory, const TObjectPtr<ACSTutorialCharacter>& InputCharacter)
{
	// if linked to a player, create the submenu
	if (InputCharacter)
	{
		if (InventorySubMenuClass)
		{
			SubMenu = CreateWidget<UInventorySubmenu>(this, InventorySubMenuClass);
			SubMenu->PlayerCharacter = InputCharacter;
			SubMenu->AddToViewport(6);
			SubMenu->SetVisibility(ESlateVisibility::Collapsed);
		}
		else
		{
			UE_LOG(LogTemp, Error, L"%s: InventorySubMenuClass was null!", *FString(__FUNCTION__));
		}
	}

	if (InputInventory)
	{
		// verify that the inventory reference is different from the incoming inventory
		if (this->InventoryReference != InputInventory)
		{
			this->InventoryReference = InputInventory;
			SubMenu->LinkedInventory = InputInventory;

			// bind the delegate so that changes in the linked inventory call RefreshInventory
			this->InventoryReference->InventoryWasUpdated.AddUObject(this, &UInventoryPanel::RefreshInventory);

			UE_LOG(LogTemp, Log, L"%s: Input inventory %s successfully linked to %s.",
			       *FString(__FUNCTION__),
			       *InputInventory->GetName(),
			       *GetName());

			// update the panel text and display its contents
			SetInfoText();
			RefreshInventory();

			bIsLinkedToInventory = true;
		}
		else
		{
			UE_LOG(LogTemp, Warning,
			       L"%s: Inventory %s is already linked to inventory: %s",
			       *FString(__FUNCTION__),
			       *GetName(),
			       *InventoryReference->GetName());
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, L"%s: Did not receive a valid input inventory component!",
		       *FString(__FUNCTION__));
	}
}

void UInventoryPanel::UnlinkFromInventory()
{
	// removes all functions from the delegate's invocation list that are bound to the specified UserObject
	const uint8 DelegatesRemoved = InventoryReference->InventoryWasUpdated.RemoveAll(this);
	if (DelegatesRemoved > 0)
	{
		UE_LOG(LogTemp, Warning, L"%s: %d InventoryWasUpdated delegates unbound from %s.",
		       *FString(__FUNCTION__), DelegatesRemoved, *GetName());
	}

	InventoryReference = nullptr;
	if (IsValid(SubMenu))
	{
		SubMenu->BeginDestroy();
	}
	bIsLinkedToInventory = false;
}

void UInventoryPanel::RefreshInventory()
{
	if (InventoryReference && InventorySlotClass)
	{
		InventoryWrapBox->ClearChildren();

		for (UItemBase* const& InventoryItem : InventoryReference->GetInventoryContents())
		{
			UInventoryItemSlot* ItemSlot = CreateWidget<UInventoryItemSlot>(this, InventorySlotClass);

			ItemSlot->InternalItemReference = InventoryItem;
			// let the item slot have access to the panel it is in so that
			// it can handle stack merging operations caused by drag & drop
			ItemSlot->OwningInventoryPanel = this;

			if (IsValid(SubMenu))
			{
				// rely on submenu being null unless explicitly set by LinkSubmenuWidget()
				ItemSlot->SubMenuReference = SubMenu;
			}

			InventoryWrapBox->AddChildToWrapBox(ItemSlot);
		}

		SetInfoText();
	}
}

void UInventoryPanel::SetInfoText() const
{
	const FString WeightInfoValue{
		FString::SanitizeFloat(InventoryReference->InventoryTotalWeight) + "/"
		+ FString::SanitizeFloat(InventoryReference->MaxWeightCapacity)
	};

	const FString CapacityInfoValue{
		FString::FromInt(InventoryReference->GetInventoryContents().Num()) + "/"
		+ FString::FromInt(InventoryReference->ItemSlotCount)
	};

	WeightInfo->SetText(FText::FromString(WeightInfoValue));
	CapacityInfo->SetText(FText::FromString(CapacityInfoValue));
}

bool UInventoryPanel::NativeOnDrop(const FGeometry& InGeometry,
                                   const FDragDropEvent& InDragDropEvent,
                                   UDragDropOperation* InOperation)
{
	const UItemDragDropOperation* ItemDragDrop = Cast<UItemDragDropOperation>(InOperation);

	if (ItemDragDrop->SourceItem && InventoryReference)
	{
		UE_LOG(LogTemp, Warning, TEXT("Detected an item drop on InventoryPanel."))

		// returning true will stop the drop operation at this widget
		return true;
	}
	// returning false will cause the drop operation to fall through to underlying widgets (if any)
	return false;
}
