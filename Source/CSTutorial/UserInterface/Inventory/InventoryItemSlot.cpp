// game
#include "UserInterface/Inventory/InventoryItemSlot.h"
#include "UserInterface/Inventory/InventoryTooltip.h"
#include "UserInterface/Inventory/DragItemVisual.h"
#include "UserInterface/Inventory/ItemDragDropOperation.h"
#include "UserInterface/Inventory/InventorySubmenu.h"
#include "UserInterface/Inventory/InventoryPanel.h"
#include "Items/ItemBase.h"

// engine
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UInventoryItemSlot::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (ToolTipClass)
	{
		UInventoryTooltip* ToolTip = CreateWidget<UInventoryTooltip>(this, ToolTipClass);
		ToolTip->InventorySlotBeingHovered = this;
		SetToolTip(ToolTip);
	}
}

void UInventoryItemSlot::NativeConstruct()
{
	Super::NativeConstruct();

	if (InternalItemReference)
	{
		switch (InternalItemReference->ItemQuality)
		{
		case EItemQuality::Shoddy:
			ItemBorder->SetBrushColor(FLinearColor::Gray);
			break;
		case EItemQuality::Common:
			ItemBorder->SetBrushColor(FLinearColor::White);
			break;
		case EItemQuality::Quality:
			ItemBorder->SetBrushColor(FLinearColor(0.0f, 0.51f, 0.169f));
			break;
		case EItemQuality::Masterwork:
			ItemBorder->SetBrushColor(FLinearColor(0.0f, 0.4f, 0.75f));
			break;
		case EItemQuality::Grandmaster:
			ItemBorder->SetBrushColor(FLinearColor(1.0f, 0.45f, 0.0f));
			break;
		default: ;
		}

		ItemIcon->SetBrushFromTexture(InternalItemReference->AssetData.Icon);

		if (InternalItemReference->NumericData.bIsStackable)
		{
			ItemQuantity->SetText(FText::AsNumber(InternalItemReference->Quantity));
		}
		else
		{
			ItemQuantity->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

FReply UInventoryItemSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		return Reply.Handled().DetectDrag(TakeWidget(), EKeys::LeftMouseButton);
	}

	if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		if (IsValid(SubMenuReference))
		{
			float MouseX;
			float MouseY;
			if (!GetOwningPlayer()->GetMousePosition(MouseX, MouseY))
			{
				UE_LOG(LogTemp, Error, L"%s: GetOwningPlayer()->GetMousePosition() returned false!",
				       *FString(__FUNCTION__));
			}
			const FVector2D WidgetPosition{MouseX, MouseY};

			SubMenuReference->SetPositionInViewport(WidgetPosition);

			// spawn the submenu just very slightly to the left and above the mouse pointer
			// so that it's easier to move the mouse down into it
			SubMenuReference->SetAlignmentInViewport(FVector2D{0.05, 0.05});

			if (!SubMenuReference->bSubMenuActive)
			{
				SubMenuReference->OriginatingItemSlot = this;
				// hide any widgets that might have been previously spawned by the submenu, such as the splitter
				SubMenuReference->HideSubmenuWidgets();
				SubMenuReference->SetVisibility(ESlateVisibility::Visible);
			}
			else if (SubMenuReference->bSubMenuActive)
			{
				// if the submenu is already active, then link it to the new item slot data
				SubMenuReference->OriginatingItemSlot = this;
				SubMenuReference->HideSubmenuWidgets();
			}

			// configure the submenu buttons for the item in this item slot
			SubMenuReference->ConfigureSubmenuButtons();

			return Reply.Handled();
		}

		return Reply.Unhandled();
	}

	return Reply.Unhandled();
}

void UInventoryItemSlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
}

void UInventoryItemSlot::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,
                                              UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	if (DragItemVisualClass)
	{
		const TObjectPtr<UDragItemVisual> DragVisual = CreateWidget<UDragItemVisual>(this, DragItemVisualClass);
		DragVisual->ItemIcon->SetBrushFromTexture(InternalItemReference->AssetData.Icon);
		DragVisual->ItemBorder->SetBrushColor(ItemBorder->GetBrushColor());

		InternalItemReference->NumericData.bIsStackable
			? DragVisual->ItemQuantity->SetText(FText::AsNumber(InternalItemReference->Quantity))
			: DragVisual->ItemQuantity->SetVisibility(ESlateVisibility::Collapsed);

		UItemDragDropOperation* DragItemOperation = NewObject<UItemDragDropOperation>();
		DragItemOperation->SourceItem = InternalItemReference;

		DragItemOperation->DefaultDragVisual = DragVisual;
		DragItemOperation->Pivot = EDragPivot::TopLeft;

		OutOperation = DragItemOperation;
	}
}

bool UInventoryItemSlot::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
                                      UDragDropOperation* InOperation)
{
	const UItemDragDropOperation* ItemDragDrop = Cast<UItemDragDropOperation>(InOperation);

	// only handle if not dropping the item on itself
	if (ItemDragDrop->SourceItem != InternalItemReference)
	{
		// only take action if the same kind of item is being dropped onto this slot
		if (ItemDragDrop->SourceItem->ID == InternalItemReference->ID)
		{
			// perform merge
			OwningInventoryPanel->InventoryReference->MergeItems(InternalItemReference, ItemDragDrop->SourceItem);
		
			return true;
			
		}
	}

	return false;
}
