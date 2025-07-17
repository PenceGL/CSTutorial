#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventorySubmenu.generated.h"

class ACSTutorialCharacter;
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
	TObjectPtr<ACSTutorialCharacter> PlayerCharacter;
	UPROPERTY()
	TObjectPtr<UInventoryItemSlot> OriginatingItemSlot;

	//#############################################################################
	// FUNCTIONS
	//#############################################################################
	FORCEINLINE bool SubMenuIsActive() const { return bSubMenuActive; };
	FORCEINLINE void SetSubmenuActive(const bool NewState) { bSubMenuActive = NewState; };
	FORCEINLINE bool SplitIsInProgress() const { return bSplitInProgress; };
	FORCEINLINE void SetAmountWidgetReference(const TObjectPtr<UAmountWidget> AmountWidgetIn);
	
	void HideSubmenuWidgets() const;
	void ConfigureSubmenuButtons();

protected:
	//#############################################################################
	// PROPERTIES & VARIABLES
	//#############################################################################
	bool bSubMenuActive;
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
	TObjectPtr<UAmountWidget> AmountWidgetReference;

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