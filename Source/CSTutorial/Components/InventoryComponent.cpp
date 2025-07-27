#include "Components/InventoryComponent.h"
#include "Items/ItemBase.h"

#define DEBUG_TO_SCREEN(Message) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange, FString(__FUNCTION__) + "(" + FString::FromInt(__LINE__) + ") " + Message);

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent(): InventoryTotalWeight(0.0), ItemSlotCount(10), MaxWeightCapacity(20.0)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
}

UItemBase* UInventoryComponent::FindMatchingItem(const TObjectPtr<UItemBase>& ItemIn) const
{
	if (ItemIn)
	{
		if (InventoryContents.Contains(ItemIn))
		{
			return ItemIn;
		}
	}
	return nullptr;
}

UItemBase* UInventoryComponent::FindNextItemByID(const TObjectPtr<UItemBase>& ItemIn) const
{
	if (ItemIn)
	{
		if (const TArray<TObjectPtr<UItemBase>>::ElementType* Result = InventoryContents.FindByKey(ItemIn))
		{
			return *Result;
		}
	}
	return nullptr;
}

UItemBase* UInventoryComponent::FindNextPartialStack(const TObjectPtr<UItemBase>& ItemIn) const
{
	if (const TArray<TObjectPtr<UItemBase>>::ElementType* Result = InventoryContents.FindByPredicate(
		[&ItemIn](const UItemBase* InventoryItem)
		{
			return InventoryItem->ID == ItemIn->ID && !InventoryItem->IsFullItemStack();
		})
	)
	{
		return *Result;
	}

	return nullptr;
}

int32 UInventoryComponent::CalculateWeightAddAmount(const TObjectPtr<UItemBase>& ItemIn, const int32 RequestedAddAmount) const
{
	const int32 WeightMaxAddAmount = FMath::FloorToInt((MaxWeightCapacity - InventoryTotalWeight) / ItemIn->GetItemSingleWeight());
	if (WeightMaxAddAmount >= RequestedAddAmount)
	{
		return RequestedAddAmount;
	}
	return WeightMaxAddAmount;
}

int32 UInventoryComponent::CalculateNumberForFullStack(const TObjectPtr<UItemBase>& StackableItem, const int32 RequestedAddAmount) const
{
	const int32 AddAmountToMakeFullStack = StackableItem->NumericData.MaxStackSize - StackableItem->Quantity;
	return FMath::Min(RequestedAddAmount, AddAmountToMakeFullStack);
}

void UInventoryComponent::MergeItems(const TObjectPtr<UItemBase>& TargetItem, const TObjectPtr<UItemBase>& SourceItem)
{
	const int32 AmountToShift = CalculateNumberForFullStack(TargetItem, SourceItem->Quantity);

	if (AmountToShift > 0)
	{
		TargetItem->SetQuantity(TargetItem->Quantity + AmountToShift);
		// weight isn't changing, so don't let HandleRemoveItem adjust it
		HandleRemoveItem(SourceItem, AmountToShift, false);
	}
}

void UInventoryComponent::HandleRemoveItem(UItemBase* ItemToRemove, const int32 AmountToRemove, const bool bAdjustWeight)
{
	if (bAdjustWeight)
		InventoryTotalWeight -= ItemToRemove->GetItemStackWeight();

	if (AmountToRemove > 0)
	{
		// adjust the quantity
		ItemToRemove->SetQuantity(ItemToRemove->Quantity - AmountToRemove);

		// if quantity is now 0, the entire item should be removed from the inventory
		if (ItemToRemove->Quantity <= 0)
		{
			InventoryContents.RemoveSingle(ItemToRemove);
		}
	}
	else
	{
		// if amount to remove is 0, assume the item entry should be removed directly
		InventoryContents.RemoveSingle(ItemToRemove);
	}

	InventoryWasUpdated.Broadcast();
}

void UInventoryComponent::SplitExistingStack(UItemBase* ItemIn, const int32 AmountToSplit)
{
	// since you're not technically *removing* an item by splitting, the split
	// process manipulates the inventory array directly

	if (InventoryContents.Num() + 1 <= ItemSlotCount)
	{
		UItemBase* SplitPortion = UItemBase::CreateItemCopy(ItemIn, this);
		SplitPortion->SetQuantity(AmountToSplit);
		InventoryContents.Add(SplitPortion);

		ItemIn->Quantity -= AmountToSplit;

		InventoryWasUpdated.Broadcast();
	}
}

FItemAddResult UInventoryComponent::HandleNonStackableItems(const TObjectPtr<UItemBase>& InputItem)
{
	// check if in the input item has valid weight
	if (FMath::IsNearlyZero(InputItem->GetItemSingleWeight()) || InputItem->GetItemSingleWeight() < 0)
	{
		return FItemAddResult::AddedNone(FText::Format(
			FText::FromString("Could not add {0} to the inventory. Item has invalid weight value."),
			InputItem->TextData.Name));
	}

	// check if the item weight will overflow weight capacity
	if (InventoryTotalWeight + InputItem->GetItemSingleWeight() > MaxWeightCapacity)
	{
		return FItemAddResult::AddedNone(FText::Format(
			FText::FromString("Could not add {0} to the inventory. Item would overflow weight limit."),
			InputItem->TextData.Name));
	}

	// check if adding one more item would overflow slot capacity
	if (InventoryContents.Num() + 1 > ItemSlotCount)
	{
		return FItemAddResult::AddedNone(FText::Format(
			FText::FromString("Could not add {0} to the inventory. All inventory slots are full."),
			InputItem->TextData.Name));
	}

	AddNewItem(InputItem, 1);

	return FItemAddResult::AddedAll(
		1, FText::Format(FText::FromString("Successfully added a single {0} to the inventory."), InputItem->TextData.Name));
}

int32 UInventoryComponent::HandleStackableItems(const TObjectPtr<UItemBase>& ItemIn, const int32 RequestedAddAmount)
{
	if (RequestedAddAmount <= 0 || FMath::IsNearlyZero(ItemIn->GetItemStackWeight()))
	{
		// invalid item data
		return 0;
	}

	int32 AmountToDistribute = RequestedAddAmount;

	// check if the input item already exists in the inventory and is not a full stack
	UItemBase* ExistingItemStack = FindNextPartialStack(ItemIn);

	// distribute item stack over existing stacks
	while (ExistingItemStack)
	{
		// calculate how many of the existing item would be needed to make the next full stack
		const int32 AmountToMakeFullStack = CalculateNumberForFullStack(ExistingItemStack, AmountToDistribute);
		// calculate how many of the AmountToMakeFullStack can actually be carried based on weight capacity
		const int32 WeightLimitAddAmount = CalculateWeightAddAmount(ExistingItemStack, AmountToMakeFullStack);

		// as long as the remaining amount of the item does not overflow weight capacity
		if (WeightLimitAddAmount > 0)
		{
			// adjust the existing items stack quantity and inventory total weight
			ExistingItemStack->SetQuantity(ExistingItemStack->Quantity + WeightLimitAddAmount);

			InventoryTotalWeight += (ExistingItemStack->GetItemSingleWeight() * WeightLimitAddAmount);

			// adjust the count to be distributed
			AmountToDistribute -= WeightLimitAddAmount;

			// if max weight capacity would be exceeded by another item, just return early
			if (InventoryTotalWeight + ExistingItemStack->GetItemSingleWeight() > MaxWeightCapacity)
			{
				InventoryWasUpdated.Broadcast();
				return RequestedAddAmount - AmountToDistribute;
			}
		}
		else if (WeightLimitAddAmount <= 0)
		{
			if (AmountToDistribute != RequestedAddAmount)
			{
				// this block will be reached if distributing an item across multiple stacks
				// and the weight limit is hit during that process
				InventoryWasUpdated.Broadcast();
				return RequestedAddAmount - AmountToDistribute;
			}

			// reached if there is a partial stack but none of it can be added at all
			return 0;
		}

		if (AmountToDistribute <= 0)
		{
			// all of the input item was distributed across existing stacks
			InventoryWasUpdated.Broadcast();
			return RequestedAddAmount;
		}

		// check if there is still another valid partial stack of the input item
		ExistingItemStack = FindNextPartialStack(ItemIn);
	}

	// no more partial stacks found, check if a new stack can be added
	if (InventoryContents.Num() + 1 <= ItemSlotCount)
	{
		// attempt to add as many from the remaining item quantity that can fit inventory weight capacity
		const int32 WeightLimitAddAmount = CalculateWeightAddAmount(ItemIn, AmountToDistribute);

		if (WeightLimitAddAmount > 0)
		{
			// if there is still more item to distribute, but weight limit has been reached
			if (WeightLimitAddAmount < AmountToDistribute)
			{
				// adjust the input item and add a new stack with as many as can be held
				AmountToDistribute -= WeightLimitAddAmount;
				ItemIn->SetQuantity(AmountToDistribute);

				// add the new partial stack with WeightLimitAddAmount as quantity 
				AddNewItem(ItemIn, WeightLimitAddAmount);
				return RequestedAddAmount - AmountToDistribute;
			}

			// otherwise, the full remainder of the stack can be added
			AddNewItem(ItemIn, AmountToDistribute);
			return RequestedAddAmount;
		}

		// reached if there is free item slots, but no remaining weight capacity
		return RequestedAddAmount - AmountToDistribute;
	}

	// can only be reached if there is no existing stack and no extra capacity slots
	return 0;
}

FItemAddResult UInventoryComponent::HandleAddItem(const TObjectPtr<UItemBase>& InputItem)
{
	const int32 InitialRequestedAddAmount = InputItem->Quantity;

	// handle non-stackable items
	if (!InputItem->NumericData.bIsStackable)
	{
		return HandleNonStackableItems(InputItem);
	}

	// handle stackable
	const int32 StackableAmountAdded = HandleStackableItems(InputItem, InitialRequestedAddAmount);

	if (StackableAmountAdded == InitialRequestedAddAmount)
	{
		return FItemAddResult::AddedAll(InitialRequestedAddAmount, FText::Format(
			                                FText::FromString("Successfully added {0} {1} to the inventory."),
			                                InitialRequestedAddAmount,
			                                InputItem->TextData.Name));
	}

	if (StackableAmountAdded < InitialRequestedAddAmount && StackableAmountAdded > 0)
	{
		return FItemAddResult::AddedPartial(StackableAmountAdded, FText::Format(
			                                    FText::FromString("Partial amount of {0} added to the inventory. Number added = {1}"),
			                                    InputItem->TextData.Name,
			                                    StackableAmountAdded));
	}

	if (StackableAmountAdded <= 0)
	{
		return FItemAddResult::AddedNone(FText::Format(
			FText::FromString("Couldn't add {0} to the inventory. No remaining inventory slots, or invalid item."),
			InputItem->TextData.Name));
	}

	return FItemAddResult::AddedNone(FText::FromString("TryAddItem fallthrough error."));
}

void UInventoryComponent::AddNewItem(const TObjectPtr<UItemBase>& Item, const int32 AmountToAdd)
{
	UItemBase* NewItem = UItemBase::CreateItemCopy(Item, this);
	NewItem->SetQuantity(AmountToAdd);
	InventoryContents.Add(NewItem);
	InventoryTotalWeight += NewItem->GetItemStackWeight();
	InventoryWasUpdated.Broadcast();
}
