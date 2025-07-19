#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventorySubmenu.generated.h"

class ACSTutorialCharacter;
class UInventoryComponent;
class UInventoryItemSlot;
class UAmountWidget;
class UButton;

/**
 * 
 */
UCLASS()
class CSTUTORIAL_API UInventorySubmenu : public UUserWidget
{
	GENERATED_BODY()

public:
	//#############################################################################
	// PROPERTIES & VARIABLES
	//#############################################################################
	UPROPERTY()
	TObjectPtr<UInventoryComponent> LinkedInventory;
	UPROPERTY()
	TObjectPtr<UInventoryItemSlot> OriginatingItemSlot;
	UPROPERTY()
	TObjectPtr<ACSTutorialCharacter> PlayerCharacter;

	bool bSubMenuActive;

	//#############################################################################
	// FUNCTIONS
	//#############################################################################
	FORCEINLINE bool SplitIsInProgress() const { return bSplitInProgress; };
	
	void HideSubmenuWidgets() const;
	void ConfigureSubmenuButtons();

protected:
	//#############################################################################
	// PROPERTIES & VARIABLES
	//#############################################################################
	bool bSplitInProgress;
	bool bItemCanBeSplit;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> UseButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ExamineButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> DropButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SplitButton;

	UPROPERTY()
	TObjectPtr<UAmountWidget> AmountWidget;

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UAmountWidget> AmountWidgetClass;

	//#############################################################################
	// FUNCTIONS
	//#############################################################################
	UFUNCTION()
	void UseButtonClicked();
	UFUNCTION()
	void ExamineButtonClicked();
	UFUNCTION()
	void DropButtonClicked();
	UFUNCTION()
	void SplitButtonClicked();
	UFUNCTION()
	void SplitConfirmed(int32 AmountToSplit);
	
	void CloseSubmenu();

	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
};