#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "CSTutorialHUD.generated.h"

class UMainMenu;
class UInteractionWidget;
class UInventorySubmenu;
class UAmountWidget;
class AContainer;
struct FInteractableData;

UCLASS()
class CSTUTORIAL_API ACSTutorialHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	//======================================================================================
	// PROPERTIES & VARIABLES
	//======================================================================================
	bool bIsMenuVisible;

	UPROPERTY()
	TObjectPtr<UInventorySubmenu> InventorySubMenu;

	UPROPERTY()
	TObjectPtr<UAmountWidget> AmountWidget;
	
	//======================================================================================
	// FUNCTIONS
	//======================================================================================
	ACSTutorialHUD();

	void DisplayMenu();
	void HideMenu();
	void ToggleMenu();

	void ShowCrosshair() const;
	void HideCrosshair() const;
	
	void ShowInteractionWidget() const;
	void HideInteractionWidget() const;
	void UpdateInteractionWidget(const FInteractableData* InteractableData) const;
	TObjectPtr<UInteractionWidget> GetInteractionWidget() { return InteractionWidget; }

	void SetTargetContainer(const TObjectPtr<AContainer> TargetContainer, const TObjectPtr<ACSTutorialCharacter> PlayerCharacter);
	void ClearTargetContainer();
	void ShowContainerInterface(const bool bModifyInputMode = false);
	void HideContainerInterface(const bool bModifyInputMode = false);
	
protected:
	//======================================================================================
	// PROPERTIES & VARIABLES
	//======================================================================================
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UMainMenu> MainMenuClass;

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UInteractionWidget> InteractionWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UUserWidget> CrosshairWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UInventorySubmenu> InventorySubMenuClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UAmountWidget> AmountWidgetClass;
	
	UPROPERTY()
	TObjectPtr<UMainMenu> MainMenuWidget;

	UPROPERTY()
	TObjectPtr<UInteractionWidget> InteractionWidget;

	UPROPERTY()
	TObjectPtr<UUserWidget> CrosshairWidget;
	
	//======================================================================================
	// FUNCTIONS
	//======================================================================================
	virtual void BeginPlay() override;

	void CreateGameWidgets();
};
