#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AmountWidget.generated.h"

DECLARE_DELEGATE_OneParam(FOnValueConfirm, int32)

class UItemBase;
class UButton;
class UTextBlock;

/**
 * 
 */
UCLASS()
class CSTUTORIAL_API UAmountWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	//#############################################################################
	// PROPERTIES & VARIABLES
	//#############################################################################
	FOnValueConfirm OnValueConfirm;

	//#############################################################################
	// FUNCTIONS
	//#############################################################################
	FORCEINLINE void SetItemReference(UItemBase* ItemIn) { ItemReference = ItemIn; };
	FORCEINLINE void SetHeaderText(const FText& InputText) const;

protected:
	//#############################################################################
	// PROPERTIES & VARIABLES
	//#############################################################################
	int32 AmountValue;
	
	UPROPERTY()
	TObjectPtr<UItemBase> ItemReference;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> HeaderText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Minus5Button;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Minus1Button;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Plus1Button;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Plus5Button;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> AmountText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CancelButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ConfirmButton;
	
	//#############################################################################
	// FUNCTIONS
	//#############################################################################
	UFUNCTION()
	void Minus5Clicked();
	UFUNCTION()
	void Minus1Clicked();
	UFUNCTION()
	void Plus1Clicked();
	UFUNCTION()
	void Plus5Clicked();
	UFUNCTION()
	void ConfirmClicked();
	UFUNCTION()
	void CancelClicked();

	void CloseAmountWidget();

	/** 
	 * Called once only at game time on non-template instances.
	 * While Construct/Destruct pertain to the underlying Slate, this is called only once for the UUserWidget.
	 * If you have one-time things to establish up-front (like binding callbacks to events on BindWidget properties), do so here.
	 */
	virtual void NativeOnInitialized() override;
	/**
	 * Called after the underlying slate widget is constructed.  Depending on how the slate object is used
	 * this event may be called multiple times due to adding and removing from the hierarchy.
	 * If you need a true called-once-when-created event, use OnInitialized.
	 */
	// virtual void NativeConstruct() override;
};
