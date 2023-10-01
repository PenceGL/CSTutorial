// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryPanel.generated.h"

class UInventoryItemSlot;
class UInventoryComponent;
class ACSTutorialCharacter;
class UWrapBox;
class UGridPanel;
class UTextBlock;

UCLASS()
class CSTUTORIAL_API UInventoryPanel : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void RefreshInventory();

	// UPROPERTY(meta=(BindWidget))
	// UWrapBox* InventoryWrapBox;

	UPROPERTY(meta=(BindWidget))
	UGridPanel* TestGridPanel;
	
	UPROPERTY(meta=(BindWidget))
	UTextBlock* WeightInfo;
	
	UPROPERTY(meta=(BindWidget))
	UTextBlock* CapacityInfo;

	UPROPERTY()
	ACSTutorialCharacter* PlayerCharacter;
	UPROPERTY()
	UInventoryComponent* InventoryReference;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UInventoryItemSlot> InventorySlotClass;
	
protected:

	void SetInfoText() const;
	virtual void NativeOnInitialized() override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
};
