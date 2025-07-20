#include "UserInterface/CSTutorialHUD.h"
#include "UserInterface/MainMenu.h"
#include "UserInterface/Interaction/InteractionWidget.h"
// #include "UserInterface/Inventory/ContainerInterface.h"

ACSTutorialHUD::ACSTutorialHUD() : bIsMenuVisible(false)
{
}

void ACSTutorialHUD::BeginPlay()
{
	Super::BeginPlay();
	CreateGameWidgets();
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
	{
		CrosshairWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void ACSTutorialHUD::HideCrosshair() const
{
	if (IsValid(CrosshairWidget))
	{
		CrosshairWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
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
		// interaction widget doesn't need to be above menus
		InteractionWidget->AddToViewport(0);
		InteractionWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		UE_LOG(LogTemp, Error, L"%s: InteractionWidgetClass was null!", *FString(__FUNCTION__));
	}

	if (CrosshairWidgetClass)
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
