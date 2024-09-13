// Copyright(c) Cavrnus. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IListElementInterface.generated.h"

class UUserWidget;

/**
 * @brief UListElementInterface class.
 *
 * This class provides the interface definition for list elements within the Cavrnus plugin.
 */
UINTERFACE(MinimalAPI)
class UListElementInterface : public UInterface
{
 GENERATED_BODY()
};

/**
 * @brief IListElementInterface class.
 *
 * This interface defines the required functionality for list elements used in the Cavrnus plugin.
 */
class IListElementInterface
{
 GENERATED_BODY()

public:
/**
* @brief Called when an entry is built.
*
* This function is called to build or initialize a list entry.
*
* @param Element The user widget representing the list entry.
*/
 virtual void EntryBuilt(UUserWidget* Element) = 0;
};