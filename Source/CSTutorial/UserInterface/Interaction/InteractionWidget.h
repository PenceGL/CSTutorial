#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractionWidget.generated.h"

class ACSTutorialCharacter;
class UTextBlock;
class UProgressBar;
struct FInteractableData;

UCLASS()
class CSTUTORIAL_API UInteractionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void UpdateWidget(const FInteractableData* InteractableData) const;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Interaction Widget | Timed Interactions")
	TObjectPtr<UProgressBar> InteractionProgressBar;
	
protected:
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Interaction Widget | Interactable Data")
	TObjectPtr<UTextBlock> KeyPressText;
	
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Interaction Widget | Interactable Data")
	TObjectPtr<UTextBlock> ActionText;
	
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Interaction Widget | Interactable Data")
	TObjectPtr<UTextBlock> NameText;
	
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Interaction Widget | Interactable Data")
	TObjectPtr<UTextBlock> QuantityText;
	
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
};
