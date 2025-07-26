#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnInventoryUpdated);

class UItemBase;

UENUM(BlueprintType)
enum class EItemAddResult : uint8
{
	IAR_NoItemAdded UMETA(DisplayName = "No item added"),
	IAR_PartialAmountItemAdded UMETA(DisplayName = "Partial amount of item added"),
	IAR_AllItemAdded UMETA(DisplayName = "All of item added")
};

USTRUCT(BlueprintType)
struct FItemAddResult
{
	GENERATED_BODY()

	FItemAddResult() :
		ActualAmountAdded(0),
		OperationResult(EItemAddResult::IAR_NoItemAdded),
		ResultMessage(FText::GetEmpty())
	{
	};

	// Actual amount of item that was added to the inventory
	int32 ActualAmountAdded;
	// Enum representing the end state of an add item operation
	EItemAddResult OperationResult;
	// Informational message that can be passed with the result
	FText ResultMessage;

	static FItemAddResult AddedNone(const FText& ErrorText)
	{
		FItemAddResult AddedNoneResult;
		AddedNoneResult.ActualAmountAdded = 0;
		AddedNoneResult.OperationResult = EItemAddResult::IAR_NoItemAdded;
		AddedNoneResult.ResultMessage = ErrorText;
		return AddedNoneResult;
	};

	static FItemAddResult AddedPartial(const int32 PartialAmountAdded, const FText& ErrorText)
	{
		FItemAddResult AddedPartialResult;
		AddedPartialResult.ActualAmountAdded = PartialAmountAdded;
		AddedPartialResult.OperationResult = EItemAddResult::IAR_PartialAmountItemAdded;
		AddedPartialResult.ResultMessage = ErrorText;
		return AddedPartialResult;
	};

	static FItemAddResult AddedAll(const int32 AmountAdded, const FText& Message)
	{
		FItemAddResult AddedAllResult;
		AddedAllResult.ActualAmountAdded = AmountAdded;
		AddedAllResult.OperationResult = EItemAddResult::IAR_AllItemAdded;
		AddedAllResult.ResultMessage = Message;
		return AddedAllResult;
	};
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CSTUTORIAL_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	//======================================================================================
	// PROPERTIES & VARIABLES
	//======================================================================================
	FOnInventoryUpdated InventoryWasUpdated;

	UPROPERTY(VisibleAnywhere, Category="Inventory")
	float InventoryTotalWeight;
	UPROPERTY(EditInstanceOnly, Category="Inventory")
	int32 ItemSlotCount;
	UPROPERTY(EditInstanceOnly, Category="Inventory")
	float MaxWeightCapacity;

	//======================================================================================
	// FUNCTIONS
	//======================================================================================
	UInventoryComponent();

	FItemAddResult HandleAddItem(const TObjectPtr<UItemBase>& InputItem);
	UItemBase* FindMatchingItem(const TObjectPtr<UItemBase>& ItemIn) const;
	UItemBase* FindNextItemByID(const TObjectPtr<UItemBase>& ItemIn) const;
	UItemBase* FindNextPartialStack(const TObjectPtr<UItemBase>& ItemIn) const;
	/** @brief Merges quantities of two items together that are already in the inventory.*/
	void MergeItems(const TObjectPtr<UItemBase>& TargetItem, const TObjectPtr<UItemBase>& SourceItem);
	/** @brief Used to remove an entire item (even if it is a stack) from the inventory in one operation.*/
	void RemoveInstanceOfItem(const TObjectPtr<UItemBase>& ItemToRemove);
	void RemoveAmountOfItem(const TObjectPtr<UItemBase>& ItemIn, int32 AmountToRemove);
	/** @brief Used to split an existing stack. Reduces the target stack quantity and adds the split amount as a new item.*/
	void SplitExistingStack(const TObjectPtr<UItemBase>& ItemIn, const int32 AmountToSplit);

	// getters
	// ------------------------------
	FORCEINLINE TArray<UItemBase*> GetInventoryContents() const { return InventoryContents; }

	// setters
	// ------------------------------
	FORCEINLINE void SetSlotsCapacity(const int32 NewSlotsCapacity) { ItemSlotCount = NewSlotsCapacity; }
	FORCEINLINE void SetWeightCapacity(const float NewWeightCapacity) { MaxWeightCapacity = NewWeightCapacity; }

protected:
	//======================================================================================
	// PROPERTIES & VARIABLES
	//======================================================================================
	UPROPERTY(VisibleAnywhere, Category="Inventory")
	TArray<TObjectPtr<UItemBase>> InventoryContents;

	//======================================================================================
	// FUNCTIONS
	//======================================================================================
	virtual void BeginPlay() override;

	FItemAddResult HandleNonStackableItems(const TObjectPtr<UItemBase>& ItemIn);
	int32 HandleStackableItems(const TObjectPtr<UItemBase>& ItemIn, int32 RequestedAddAmount);
	int32 CalculateWeightAddAmount(const TObjectPtr<UItemBase>& ItemIn, int32 RequestedAddAmount) const;
	int32 CalculateNumberForFullStack(const TObjectPtr<UItemBase>& StackableItem, int32 RequestedAddAmount) const;

	void AddNewItem(const TObjectPtr<UItemBase>& Item, const int32 AmountToAdd);
};
