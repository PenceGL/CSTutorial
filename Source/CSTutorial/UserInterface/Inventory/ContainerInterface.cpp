// game
#include "ContainerInterface.h"
#include "World/Container.h"
#include "Player/CSTutorialCharacter.h"
#include "UserInterface/Inventory/InventoryPanel.h"
#include "Components/InventoryComponent.h"

// engine
#include "Components/Button.h"

void UContainerInterface::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	CloseButton->OnClicked.AddDynamic(this, &UContainerInterface::HideContainerInterface);
}

void UContainerInterface::ClearTargetContainer()
{
	TargetContainer = nullptr;
	ContainerInventoryPanel->UnlinkFromInventory();
	UE_LOG(LogTemp, Warning, L"%s: Nulling TargetContainer reference in %s.", *FString(__FUNCTION__), *GetName());
}

void UContainerInterface::HideContainerInterface()
{
	// broadcast to close the interface from within the HUD to allow it to change game input mode
	// and do any other higher level functions needed
	if (CloseContainerInterface.ExecuteIfBound(true))
	{
		UE_LOG(LogTemp, Log,
		       L"%s: Closing container interface via CloseContainerInterface delegate, no references nulled.",
		       *FString(__FUNCTION__));
	}
}

void UContainerInterface::LinkContainerInterface(AContainer* InputContainer, ACSTutorialCharacter* PlayerCharacter)
{
	if (InputContainer && PlayerCharacter)
	{
		TargetContainer = InputContainer;

		// link the containers inventory to the container side of the interface
		ContainerInventoryPanel->LinkToInventory(TargetContainer->ContainerInventory);

		if (!PlayerCharacter->GetInventory()->InventoryWasUpdated.IsBoundToObject(PlayerInventoryPanel))
		{
			// link the players inventory to the player side of the interface
			// ensure the player reference is passed in (instead of default nullptr) so
			// that a submenu is created for the player inventory
			PlayerInventoryPanel->LinkToInventory(PlayerCharacter->GetInventory(), PlayerCharacter);
		}
	}
}
