#include "Items/ItemBase.h"
#include "Components/InventoryComponent.h"

UItemBase::UItemBase() : bIsCopy(false), bIsPickup(false)
{
}

void UItemBase::ResetItemFlags()
{
	this->bIsCopy = false;
	this->bIsPickup = false;
}

UItemBase* UItemBase::CreateItemCopy() const
{
	UItemBase* ItemCopy = NewObject<UItemBase>(StaticClass());

	ItemCopy->ID = this->ID;
	ItemCopy->Quantity = this->Quantity;
	ItemCopy->ItemQuality = this->ItemQuality;
	ItemCopy->ItemType = this->ItemType;
	ItemCopy->TextData = this->TextData;
	ItemCopy->NumericData = this->NumericData;
	ItemCopy->ItemStatistics = this->ItemStatistics;
	ItemCopy->AssetData = this->AssetData;
	
	ItemCopy->bIsCopy = true;

	return ItemCopy;
}

void UItemBase::SetQuantity(const int32 NewQuantity)
{
	if (NewQuantity != this->Quantity)
	{
		Quantity = FMath::Clamp(NewQuantity, 0, this->NumericData.bIsStackable ? this->NumericData.MaxStackSize : 1);

		// if the item belongs to an inventory and its quantity is set to 0, remove it
		if (this->OwningInventory)
		{
			if (this->Quantity <= 0)
			{
				this->OwningInventory->RemoveSingleInstanceOfItem(this);
			}
		}
		else if (!this->bIsPickup)
		{
			// if the item is a pickup, it should not have an owning inventory
			// all other cases should trigger a warning if no owning inventory found
			UE_LOG(LogTemp, Warning, TEXT("ItemBase OwningInventory was null!"));
		}
	}
}

void UItemBase::Use(ACSTutorialCharacter* PlayerCharacter)
{
}
