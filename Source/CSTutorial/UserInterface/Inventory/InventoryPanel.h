#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryPanel.generated.h"

class UInventoryItemSlot;
class UInventoryComponent;
class UInventorySubmenu;
class ACSTutorialCharacter;
class UWrapBox;
class UGridPanel;
class UTextBlock;

UCLASS()
class CSTUTORIAL_API UInventoryPanel : public UUserWidget
{
	GENERATED_BODY()

public:
	void LinkSubmenuWidget();
	
	UFUNCTION()
	void RefreshInventory();

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWrapBox> InventoryWrapBox;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> WeightInfo;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> CapacityInfo;

	UPROPERTY()
	TObjectPtr<ACSTutorialCharacter> PlayerCharacter;
	
	UPROPERTY()
	TObjectPtr<UInventoryComponent> InventoryReference;

	UPROPERTY()
	TObjectPtr<UInventorySubmenu> SubMenuReference;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UInventoryItemSlot> InventorySlotClass;
	
protected:

	void SetInfoText() const;
	virtual void NativeOnInitialized() override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
};
