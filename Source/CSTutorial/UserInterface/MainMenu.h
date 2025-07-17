#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenu.generated.h"

class ACSTutorialCharacter;

UCLASS()
class CSTUTORIAL_API UMainMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TObjectPtr<ACSTutorialCharacter> PlayerCharacter;

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
};
