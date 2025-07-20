#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CSTutorialGameMode.generated.h"

/**
 *  Simple GameMode for a third person game
 */
UCLASS(abstract)
class ACSTutorialGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	/** Constructor */
	ACSTutorialGameMode();
};
