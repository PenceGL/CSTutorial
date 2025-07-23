#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/InteractionInterface.h"
#include "Container.generated.h"

class UInventoryComponent;
class UDataTable;
class USphereComponent;

UCLASS()
class CSTUTORIAL_API AContainer : public AActor, public IInteractionInterface
{
	GENERATED_BODY()

public:
	//#############################################################################
	// PROPERTIES & VARIABLES
	//#############################################################################
	UPROPERTY(EditInstanceOnly)
	TObjectPtr<UInventoryComponent> ContainerInventory;

	//#############################################################################
	// FUNCTIONS
	//#############################################################################
	AContainer();
	
	virtual void BeginFocus() override;
	virtual void EndFocus() override;
	virtual void Interact(ACSTutorialCharacter* PlayerCharacter) override;

protected:
	//#############################################################################
	// PROPERTIES & VARIABLES
	//#############################################################################
	UPROPERTY(EditInstanceOnly, Category = "Container | Mesh")
	TObjectPtr<UStaticMeshComponent> ContainerMesh;

	UPROPERTY(EditInstanceOnly, Category = "Container | Initialization")
	FInteractableData InstanceInteractableData;

	UPROPERTY(EditInstanceOnly, Category = "Container | Initialization")
	int32 AmountOfInventorySlots;

	UPROPERTY(EditInstanceOnly, Category = "Container | Initialization")
	float MaxWeightCapacity;

	UPROPERTY()
	TObjectPtr<ACSTutorialCharacter> PlayerCharacterReference;

	UPROPERTY()
	TObjectPtr<USphereComponent> ContainerExitSphere;

	UPROPERTY(EditInstanceOnly, Category = "Container | Initialization")
	float ContainerExitRadius;

	// Reminder:
	// UPROPERTY(EditInstanceOnly, Category = "Container | Loot Initialization")
	// TArray<TObjectPtr<UDataTable>> LootTables;
	//
	// UPROPERTY(EditInstanceOnly, Category = "Container | Loot Initialization")
	// FIntPoint RandomNumberRange;
	//
	// UPROPERTY(EditInstanceOnly, Category = "Container | Loot Initialization")
	// bool bInitWithRandomContents;

	//#############################################################################
	// FUNCTIONS
	//#############################################################################
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnContainerRadiusExit(UPrimitiveComponent* OverlappedComponent,
	                           AActor* OtherActor,
	                           UPrimitiveComponent* OtherComp,
	                           int32 OtherBodyIndex);
};
