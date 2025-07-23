// game
#include "Container.h"
#include "Components/InventoryComponent.h"
#include "Player/CSTutorialCharacter.h"

// engine
#include "Components/SphereComponent.h"

AContainer::AContainer() : ContainerExitRadius(225.0f)
{
	PrimaryActorTick.bCanEverTick = false;

	ContainerMesh = CreateDefaultSubobject<UStaticMeshComponent>(L"ContainerMesh");
	SetRootComponent(ContainerMesh);

	ContainerMesh->SetCollisionObjectType(ECC_GameTraceChannel1);

	ContainerInventory = CreateDefaultSubobject<UInventoryComponent>(L"ContainerInventory");

	ContainerExitSphere = CreateDefaultSubobject<USphereComponent>(L"ContainerExitSphere");
	ContainerExitSphere->SetupAttachment(GetRootComponent());
	ContainerExitSphere->InitSphereRadius(ContainerExitRadius);
}

void AContainer::BeginPlay()
{
	Super::BeginPlay();

	ContainerExitSphere->InitSphereRadius(ContainerExitRadius);

	ContainerInventory->SetSlotsCapacity(AmountOfInventorySlots);
	ContainerInventory->SetWeightCapacity(MaxWeightCapacity);

	InteractableData = InstanceInteractableData;

	ContainerExitSphere->OnComponentEndOverlap.AddDynamic(this, &AContainer::OnContainerRadiusExit);
}

void AContainer::BeginFocus()
{
	if (ContainerMesh)
	{
		ContainerMesh->SetRenderCustomDepth(true);
	}
}

void AContainer::EndFocus()
{
	if (ContainerMesh)
	{
		ContainerMesh->SetRenderCustomDepth(false);
	}
}

void AContainer::Interact(ACSTutorialCharacter* PlayerCharacter)
{
	if (!IsValid(PlayerCharacter))
	{
		// when a character interacts, store a reference to them so that
		// the container interface can then be closed via their HUD when they walk away with it open
		if (PlayerCharacter)
		{
			UE_LOG(LogTemp, Log, L"%s: Successfully obtained player character reference.", *FString(__FUNCTION__))
		}
	}
}

void AContainer::OnContainerRadiusExit(UPrimitiveComponent* OverlappedComponent,
                                       AActor* OtherActor,
                                       UPrimitiveComponent* OtherComp,
                                       int32 OtherBodyIndex)
{
	if (OtherActor->IsA(ACSTutorialCharacter::StaticClass()))
	{
		if (IsValid(PlayerCharacterReference))
		{
			PlayerCharacterReference->ExitContainerRadius();
			PlayerCharacterReference = nullptr;
		}
	}
}
