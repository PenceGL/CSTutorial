// game
#include "ContainerInterface.h"
#include "World/Container.h"
#include "UserInterface/Inventory/InventoryPanel.h"

// engine
#include "Components/Button.h"

void UContainerInterface::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	CloseButton->OnClicked.AddDynamic(this, &UContainerInterface::HideContainerInterface);
}

void UContainerInterface::ClearTargetContainer()
{
	if (ContainerInventoryPanel->bIsLinkedToInventory)
	{
		TargetContainer = nullptr;
		ContainerInventoryPanel->UnlinkFromInventory();
		UE_LOG(LogTemp, Warning, L"%s: Nulling TargetContainer reference in %s.", *FString(__FUNCTION__), *GetName());
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst - can't bind const functions to delegates
void UContainerInterface::HideContainerInterface()
{
	// broadcast to close the interface from the HUD to allow it to change game input mode
	// and do any other higher level functions needed
	if (CloseContainerInterface.ExecuteIfBound(true))
	{
		UE_LOG(LogTemp, Log,
		       L"%s: Closing container interface via CloseContainerInterface delegate, no references nulled.",
		       *FString(__FUNCTION__));
	}
}

void UContainerInterface::LinkContainerInterface(AContainer* InputContainer)
{
	TargetContainer = InputContainer;
	ContainerInventoryPanel->LinkToInventory(TargetContainer->ContainerInventory);
}
