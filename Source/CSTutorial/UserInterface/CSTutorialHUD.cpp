#include "UserInterface/CSTutorialHUD.h"
#include "UserInterface/MainMenu.h"
#include "UserInterface/Interaction/InteractionWidget.h"
#include "UserInterface/Inventory/AmountWidget.h"
// #include "UserInterface/Inventory/ContainerInterface.h"
#include "UserInterface/Inventory/InventorySubMenu.h"

ACSTutorialHUD::ACSTutorialHUD()
{
}

void ACSTutorialHUD::BeginPlay()
{
	Super::BeginPlay();
	CreateGameWidgets();
	
	if (InventorySubMenu && AmountWidget)
	{
		InventorySubMenu->SetAmountWidgetReference(AmountWidget);
	}
	else
	{
		UE_LOG(LogTemp, Error, L"%s: AmountWidget was null when attempting to link it to the InventorySubMenu!",
			   *FString(__FUNCTION__));
	}
}

void ACSTutorialHUD::DisplayMenu()
{
	if (MainMenuWidget)
	{
		bIsMenuVisible = true;
		MainMenuWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void ACSTutorialHUD::HideMenu()
{
	if (MainMenuWidget)
	{
		bIsMenuVisible = false;
		MainMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void ACSTutorialHUD::ToggleMenu()
{
	if (bIsMenuVisible)
	{
		HideMenu();

		const FInputModeGameOnly InputMode;
		GetOwningPlayerController()->SetInputMode(InputMode);
		GetOwningPlayerController()->SetShowMouseCursor(false);
	}
	else
	{
		DisplayMenu();
		const FInputModeGameAndUI InputMode;
		GetOwningPlayerController()->SetInputMode(InputMode);
		GetOwningPlayerController()->SetShowMouseCursor(true);
	}
}

void ACSTutorialHUD::ShowCrosshair() const
{
	if (IsValid(CrosshairWidget))
		CrosshairWidget->SetVisibility(ESlateVisibility::Visible);
}

void ACSTutorialHUD::HideCrosshair() const
{
	if (IsValid(CrosshairWidget))
		CrosshairWidget->SetVisibility(ESlateVisibility::Collapsed);
}

void ACSTutorialHUD::ShowInteractionWidget() const
{
	if (IsValid(InteractionWidget))
	{
		InteractionWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void ACSTutorialHUD::HideInteractionWidget() const
{
	if (IsValid(InteractionWidget))
	{
		InteractionWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void ACSTutorialHUD::UpdateInteractionWidget(const FInteractableData* InteractableData) const
{
	if (IsValid(InteractionWidget))
	{
		if (InteractionWidget->GetVisibility() == ESlateVisibility::Collapsed)
		{
			InteractionWidget->SetVisibility(ESlateVisibility::Visible);
		}

		InteractionWidget->UpdateWidget(InteractableData);
	}
}

void ACSTutorialHUD::SetTargetContainer(const TObjectPtr<AContainer> TargetContainer, const TObjectPtr<ACSTutorialCharacter> PlayerCharacter)
{
}

void ACSTutorialHUD::ClearTargetContainer()
{
}

void ACSTutorialHUD::ShowContainerInterface(const bool bModifyInputMode)
{
}

void ACSTutorialHUD::HideContainerInterface(const bool bModifyInputMode)
{
}

void ACSTutorialHUD::CreateGameWidgets()
{
	if (MainMenuClass)
	{
		MainMenuWidget = CreateWidget<UMainMenu>(GetWorld(), MainMenuClass);
		MainMenuWidget->AddToViewport(5);
		MainMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		UE_LOG(LogTemp, Error, L"%s: MainMenuWidgetClass was null!", *FString(__FUNCTION__));
	}

	if (InteractionWidgetClass)
	{
		InteractionWidget = CreateWidget<UInteractionWidget>(GetWorld(), InteractionWidgetClass);
		InteractionWidget->AddToViewport(-1);
		InteractionWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		UE_LOG(LogTemp, Error, L"%s: InteractionWidgetClass was null!", *FString(__FUNCTION__));
	}

	if (CrosshairWidgetClass)
	{
		CrosshairWidget = CreateWidget<UUserWidget>(GetWorld(), CrosshairWidgetClass);
		CrosshairWidget->AddToViewport();
		CrosshairWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		UE_LOG(LogTemp, Error, L"%s: CrosshairWidgetClass was null!", *FString(__FUNCTION__));
	}

	if (AmountWidgetClass)
	{
		AmountWidget = CreateWidget<UAmountWidget>(GetWorld(), AmountWidgetClass);
		AmountWidget->AddToViewport(4);
		AmountWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		UE_LOG(LogTemp, Error, L"%s: AmountWidgetClass was null!", *FString(__FUNCTION__));
	}

	if (InventorySubMenuClass)
	{
		InventorySubMenu = CreateWidget<UInventorySubmenu>(GetWorld(), InventorySubMenuClass);
		InventorySubMenu->AddToViewport(3);
		InventorySubMenu->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		UE_LOG(LogTemp, Error, L"%s: InventorySubMenuClass was null!", *FString(__FUNCTION__));
	}
}