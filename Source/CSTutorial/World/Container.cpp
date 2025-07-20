#include "Container.h"

AContainer::AContainer()
{
}

void AContainer::BeginPlay()
{
	Super::BeginPlay();
}

void AContainer::BeginFocus()
{
	IInteractionInterface::BeginFocus();
}

void AContainer::EndFocus()
{
	IInteractionInterface::EndFocus();
}

void AContainer::Interact(ACSTutorialCharacter* PlayerCharacter)
{
	IInteractionInterface::Interact(PlayerCharacter);
}