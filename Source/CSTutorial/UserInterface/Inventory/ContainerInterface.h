#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ContainerInterface.generated.h"

DECLARE_DELEGATE_OneParam(FCloseContainerInterface, bool)

class ACSTutorialCharacter;
class AContainer;
class UInventoryComponent;
class UInventoryPanel;
class UTextBlock;
class UButton;

/**
 * 
 */
UCLASS()
class CSTUTORIAL_API UContainerInterface : public UUserWidget
{
	GENERATED_BODY()

public:
	//#############################################################################
	// PROPERTIES & VARIABLES
	//#############################################################################
	FCloseContainerInterface CloseContainerInterface;
	
	UPROPERTY()
	AContainer* TargetContainer;

	//#############################################################################
	// FUNCTIONS
	//#############################################################################
	void LinkContainerInterface(AContainer* InputContainer);
	void ClearTargetContainer();

	UFUNCTION()
	void HideContainerInterface();

protected:
	//#############################################################################
	// PROPERTIES & VARIABLES
	//#############################################################################
	/** @brief The inventory panel that will display the container inventory contents when the container interface is open.*/
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInventoryPanel> ContainerInventoryPanel;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> LootAllButton;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CloseButton;

	//#############################################################################
	// FUNCTIONS
	//#############################################################################
	virtual void NativeOnInitialized() override;
};
