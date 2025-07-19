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
	{};

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

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CSTUTORIAL_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	//======================================================================================
	// PROPERTIES & VARIABLES
	//======================================================================================
	FOnInventoryUpdated OnInventoryUpdated;
	
	//======================================================================================
	// FUNCTIONS
	//======================================================================================
	UInventoryComponent();

	UFUNCTION(Category = "Inventory")
	FItemAddResult HandleAddItem(UItemBase* InputItem);
	
	UFUNCTION(Category = "Inventory")
	UItemBase* FindMatchingItem(UItemBase* ItemIn) const;
	UFUNCTION(Category = "Inventory")
	UItemBase* FindNextItemByID(UItemBase* ItemIn) const;
	UFUNCTION(Category = "Inventory")
	UItemBase* FindNextPartialStack(UItemBase* ItemIn) const;

	/** @brief Used to remove an entire item from the inventory in one operation.*/
	UFUNCTION(Category = "Inventory")
	void RemoveInstanceOfItem(UItemBase* ItemToRemove);

	/** @brief Used to remove a partial amount of an item from the inventory.*/
	UFUNCTION(Category = "Inventory")
	void RemoveAmountOfItem(UItemBase* ItemIn, int32 DesiredAmountToRemove);

	/** @brief Used to split an existing stack. Reduces the target stack quantity and adds the split amount as a new item.*/
	UFUNCTION(Category = "Inventory")
	void SplitExistingStack(UItemBase* ItemIn, const int32 AmountToSplit);

	// getters
	// ------------------------------
	UFUNCTION(Category = "Inventory")
	FORCEINLINE float GetInventoryTotalWeight() const { return InventoryTotalWeight; };
	UFUNCTION(Category = "Inventory")
	FORCEINLINE float GetWeightCapacity() const { return InventoryWeightCapacity; };
	UFUNCTION(Category = "Inventory")
	FORCEINLINE int32 GetSlotsCapacity() const { return InventorySlotsCapacity; };
	UFUNCTION(Category = "Inventory")
	FORCEINLINE TArray<UItemBase*> GetInventoryContents() const { return InventoryContents; };
	// setters
	// ------------------------------
	UFUNCTION(Category = "Inventory")
	FORCEINLINE void SetSlotsCapacity(const int32 NewSlotsCapacity) { InventorySlotsCapacity = NewSlotsCapacity; };
	UFUNCTION(Category = "Inventory")
	FORCEINLINE void SetWeightCapacity(const float NewWeightCapacity) { InventoryWeightCapacity = NewWeightCapacity; };
	
protected:
	//======================================================================================
	// PROPERTIES & VARIABLES
	//======================================================================================
	UPROPERTY(VisibleAnywhere, Category="Inventory")
	float InventoryTotalWeight;
	UPROPERTY(EditInstanceOnly, Category="Inventory")
	int32 InventorySlotsCapacity;
	UPROPERTY(EditInstanceOnly, Category="Inventory")
	float InventoryWeightCapacity;

	UPROPERTY(VisibleAnywhere, Category="Inventory")
	TArray<TObjectPtr<UItemBase>> InventoryContents;
	
	//======================================================================================
	// FUNCTIONS
	//======================================================================================
	virtual void BeginPlay() override;

	FItemAddResult HandleNonStackableItems(UItemBase* ItemIn);
	int32 HandleStackableItems(UItemBase* ItemIn, int32 RequestedAddAmount);
	int32 CalculateWeightAddAmount(const UItemBase* ItemIn, int32 RequestedAddAmount) const;
	int32 CalculateNumberForFullStack(const UItemBase* StackableItem, int32 InitialRequestedAddAmount) const;

	void AddNewItem(const UItemBase* Item, const int32 AmountToAdd);
};
