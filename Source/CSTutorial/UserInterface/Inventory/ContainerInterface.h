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
	TObjectPtr<AContainer> TargetContainer;

	//#############################################################################
	// FUNCTIONS
	//#############################################################################
	void LinkContainerInterface(const TObjectPtr<AContainer> InputContainer,
								const TObjectPtr<ACSTutorialCharacter> PlayerCharacter);
	void ClearTargetContainer();

	UFUNCTION()
	void HideContainerInterface();

protected:
	//#############################################################################
	// PROPERTIES & VARIABLES
	//#############################################################################
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ContainerName;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInventoryPanel> ContainerInventoryPanel;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInventoryPanel> ContainerPlayerInventoryPanel;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CloseButton;

	//#############################################################################
	// FUNCTIONS
	//#############################################################################
	virtual void NativeOnInitialized() override;
};
