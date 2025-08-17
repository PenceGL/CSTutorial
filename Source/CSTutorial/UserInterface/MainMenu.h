#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenu.generated.h"

class ACSTutorialCharacter;
class UInventoryPanel;
class UContainerInterface;

UCLASS()
class CSTUTORIAL_API UMainMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TObjectPtr<ACSTutorialCharacter> PlayerCharacter;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UInventoryPanel> PlayerInventory;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UContainerInterface> ContainerInterface;

protected:
	void NativeOnInitialized() override;
	void NativeConstruct() override;
	bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
};
