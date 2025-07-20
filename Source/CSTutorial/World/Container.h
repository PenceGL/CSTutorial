#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/InteractionInterface.h"
#include "Container.generated.h"

UCLASS()
class CSTUTORIAL_API AContainer : public AActor, public IInteractionInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
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

	virtual void BeginPlay() override;
};
