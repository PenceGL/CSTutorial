#include "UserInterface/CSTutorialHUD.h"
#include "UserInterface/MainMenu.h"
#include "UserInterface/Interaction/InteractionWidget.h"
#include "UserInterface/Inventory/ContainerInterface.h"

ACSTutorialHUD::ACSTutorialHUD() :
	bMainMenuOpen(false),
	bContainerInterfaceOpen(false)
{
}

void ACSTutorialHUD::BeginPlay()
{
	Super::BeginPlay();
	CreateGameWidgets();
}

bool ACSTutorialHUD::HasAnyMenuOpen() const
{
	return bMainMenuOpen || bContainerInterfaceOpen;
}

void ACSTutorialHUD::DisplayMenu()
{
	bMainMenuOpen = true;
	MainMenuWidget->SetVisibility(ESlateVisibility::Visible);
}

void ACSTutorialHUD::HideMenu()
{
	bMainMenuOpen = false;
	MainMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
}

void ACSTutorialHUD::ToggleMenu()
{
	if (bContainerInterfaceOpen)
	{
		HideContainerInterface();
	}

	if (bMainMenuOpen)
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
	CrosshairWidget->SetVisibility(ESlateVisibility::Visible);
}

void ACSTutorialHUD::HideCrosshair() const
{
	CrosshairWidget->SetVisibility(ESlateVisibility::Collapsed);
}

void ACSTutorialHUD::ShowInteractionWidget()
{
	bInteractionWidgetVisible = true;
	InteractionWidget->SetVisibility(ESlateVisibility::Visible);
}

void ACSTutorialHUD::HideInteractionWidget()
{
	bInteractionWidgetVisible = false;
	InteractionWidget->SetVisibility(ESlateVisibility::Collapsed);
}

void ACSTutorialHUD::UpdateInteractionWidget(const FInteractableData* InteractableData) const
{
	if (InteractionWidget->GetVisibility() == ESlateVisibility::Collapsed)
	{
		InteractionWidget->SetVisibility(ESlateVisibility::Visible);
	}

	InteractionWidget->UpdateWidget(InteractableData);
}

void ACSTutorialHUD::SetTargetContainer(AContainer* TargetContainer, ACSTutorialCharacter* PlayerCharacter)
{
	if (ContainerInterface->TargetContainer != TargetContainer)
	{
		ContainerInterface->LinkContainerInterface(TargetContainer, PlayerCharacter);
	}
	else
	{
		UE_LOG(LogTemp, Warning, L"%s: ContainerInterface is already linked to this container.", *FString(__FUNCTION__));
	}

	// if container interface is not open, but menu is, toggle the menu to hide it
	if (!bContainerInterfaceOpen && bMainMenuOpen)
	{
		ToggleMenu();
	}

	ShowContainerInterface(true);
}

void ACSTutorialHUD::ClearTargetContainer()
{
	HideContainerInterface(true);
	ContainerInterface->ClearTargetContainer();
}

void ACSTutorialHUD::ShowContainerInterface(const bool bModifyInputMode)
{
	bContainerInterfaceOpen = true;
	ContainerInterface->SetVisibility(ESlateVisibility::Visible);
	HideInteractionWidget();

	if (bModifyInputMode)
	{
		const FInputModeGameAndUI InputMode;
		GetOwningPlayerController()->SetInputMode(InputMode);
		GetOwningPlayerController()->SetShowMouseCursor(true);
	}
}

void ACSTutorialHUD::HideContainerInterface(const bool bModifyInputMode)
{
	bContainerInterfaceOpen = false;
	ContainerInterface->SetVisibility(ESlateVisibility::Collapsed);

	if (bModifyInputMode)
	{
		const FInputModeGameOnly InputMode;
		GetOwningPlayerController()->SetInputMode(InputMode);
		GetOwningPlayerController()->SetShowMouseCursor(false);
	}
}


void ACSTutorialHUD::CreateGameWidgets()
{
	if (IsValid(MainMenuClass))
	{
		MainMenuWidget = CreateWidget<UMainMenu>(GetWorld(), MainMenuClass);
		MainMenuWidget->AddToViewport(5);
		MainMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		UE_LOG(LogTemp, Error, L"%s: MainMenuWidgetClass was null!", *FString(__FUNCTION__));
	}

	if (IsValid(InteractionWidgetClass))
	{
		InteractionWidget = CreateWidget<UInteractionWidget>(GetWorld(), InteractionWidgetClass);
		// interaction widget doesn't need to be above menus
		InteractionWidget->AddToViewport(0);
		InteractionWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		UE_LOG(LogTemp, Error, L"%s: InteractionWidgetClass was null!", *FString(__FUNCTION__));
	}

	if (IsValid(CrosshairWidgetClass))
	{
		CrosshairWidget = CreateWidget<UUserWidget>(GetWorld(), CrosshairWidgetClass);
		// crosshair is conditional and always in center of screen, so it won't conflict with interaction widget
		CrosshairWidget->AddToViewport(0);
		CrosshairWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		UE_LOG(LogTemp, Error, L"%s: CrosshairWidgetClass was null!", *FString(__FUNCTION__));
	}

	if (IsValid(ContainerInterfaceClass))
	{
		ContainerInterface = CreateWidget<UContainerInterface>(GetWorld(), ContainerInterfaceClass);
		ContainerInterface->AddToViewport(5);
		ContainerInterface->SetVisibility(ESlateVisibility::Collapsed);
		// bind the close button on the container interface to the HideContainerInterface HUD object
		ContainerInterface->CloseContainerInterface.BindUObject(this, &ACSTutorialHUD::HideContainerInterface);
	}
	else
	{
		UE_LOG(LogTemp, Error, L"%s: ContainerInterfaceClass was null!", *FString(__FUNCTION__));
	}
}
