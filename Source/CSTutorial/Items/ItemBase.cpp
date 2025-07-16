#include "Items/ItemBase.h"

UItemBase::UItemBase()
{
}

UItemBase* UItemBase::CreateItemCopy(const UItemBase* ItemToCopy, UObject* NewOuter)
{
	UItemBase* ItemCopy = NewObject<UItemBase>(NewOuter, StaticClass());

	ItemCopy->ID = ItemToCopy->ID;
	ItemCopy->Quantity = ItemToCopy->Quantity;
	ItemCopy->ItemQuality = ItemToCopy->ItemQuality;
	ItemCopy->ItemType = ItemToCopy->ItemType;
	ItemCopy->TextData = ItemToCopy->TextData;
	ItemCopy->NumericData = ItemToCopy->NumericData;
	ItemCopy->ItemStatistics = ItemToCopy->ItemStatistics;
	ItemCopy->AssetData = ItemToCopy->AssetData;

	return ItemCopy;
}

void UItemBase::SetQuantity(const int32 NewQuantity)
{
	if (NewQuantity != this->Quantity)
	{
		Quantity = FMath::Clamp(NewQuantity, 0, this->NumericData.bIsStackable ? this->NumericData.MaxStackSize : 1);
	}
}

void UItemBase::Use(ACSTutorialCharacter* PlayerCharacter)
{
}
