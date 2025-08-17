#include "UserInterface/CSTutorialHUD.h"
#include "UserInterface/MainMenu.h"
#include "UserInterface/Interaction/InteractionWidget.h"
#include "UserInterface/Inventory/ContainerInterface.h"

ACSTutorialHUD::ACSTutorialHUD() :
	bMainMenuOpen(false),
	bContainerInterfaceOpen(false),
	bInteractionWidgetVisible(false)
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

void ACSTutorialHUD::SetTargetContainer(AContainer* TargetContainer)
{
	if (MainMenuWidget->ContainerInterface->TargetContainer != TargetContainer)
	{
		MainMenuWidget->ContainerInterface->LinkContainerInterface(TargetContainer);
	}
	else
	{
		UE_LOG(LogTemp, Warning, L"%s: ContainerInterface is already linked to this container.", *FString(__FUNCTION__));
	}

	if (!bMainMenuOpen)
	{
		ToggleMenu();
	}
	
	ShowContainerInterface(true);
}

void ACSTutorialHUD::ClearTargetContainer()
{
	HideContainerInterface(true);
	MainMenuWidget->ContainerInterface->ClearTargetContainer();
}

void ACSTutorialHUD::ShowContainerInterface(const bool bModifyInputMode)
{
	bContainerInterfaceOpen = true;
	MainMenuWidget->ContainerInterface->SetVisibility(ESlateVisibility::Visible);
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
	MainMenuWidget->ContainerInterface->SetVisibility(ESlateVisibility::Collapsed);

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
		MainMenuWidget->ContainerInterface->CloseContainerInterface.BindUObject(this, &ACSTutorialHUD::HideContainerInterface);
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
}
