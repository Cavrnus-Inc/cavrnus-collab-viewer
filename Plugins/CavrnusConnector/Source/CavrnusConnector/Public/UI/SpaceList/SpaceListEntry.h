// Copyright(c) Cavrnus. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SpaceListEntry.generated.h"

/**
 * @brief Abstract base class for an entry in a space list within the CavrnusConnector plugin.
 *
 * This class provides the base functionality for a widget representing an entry in a list of spaces.
 * It is intended to be extended with additional functionality specific to the application.
 */

UCLASS(Abstract)
class CAVRNUSCONNECTOR_API USpaceListEntry : public UUserWidget
{
	GENERATED_BODY()
};
