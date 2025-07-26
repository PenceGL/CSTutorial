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
	bool bIsLinkedToInventory;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWrapBox> InventoryWrapBox;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> WeightInfo;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> CapacityInfo;

	UPROPERTY()
	TObjectPtr<UInventoryComponent> InventoryReference;

	UPROPERTY()
	TObjectPtr<UInventorySubmenu> SubMenu;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UInventoryItemSlot> InventorySlotClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UInventorySubmenu> InventorySubMenuClass;

	UFUNCTION()
	void RefreshInventory();

	void LinkToInventory(UInventoryComponent* InputInventory, ACSTutorialCharacter* InputCharacter = nullptr);
	void UnlinkFromInventory();

protected:
	void SetInfoText() const;
	virtual void NativeOnInitialized() override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
};
