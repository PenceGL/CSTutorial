#include "UserInterface/MainMenu.h"

#include "Inventory/InventoryPanel.h"
#include "UserInterface/Inventory/ItemDragDropOperation.h"
#include "Player/CSTutorialCharacter.h"

void UMainMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UMainMenu::NativeConstruct()
{
	Super::NativeConstruct();

	PlayerCharacter = Cast<ACSTutorialCharacter>(GetOwningPlayerPawn());
	PlayerInventory->LinkToInventory(PlayerCharacter->GetInventory(), PlayerCharacter);
}

bool UMainMenu::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	const UItemDragDropOperation* ItemDragDrop = Cast<UItemDragDropOperation>(InOperation);

	if (PlayerCharacter && ItemDragDrop->SourceItem)
	{
		PlayerCharacter->DropItem(ItemDragDrop->SourceItem);
		return true;
	}
	return false;
}
