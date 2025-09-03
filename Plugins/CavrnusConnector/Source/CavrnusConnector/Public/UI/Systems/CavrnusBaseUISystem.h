// // Copyright (c) 2025 Cavrnus. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CavrnusBaseUISystem.generated.h"

class UCavrnusBaseUserWidget;
class ICavrnusWidgetDisplayer;
class UCavrnusWidgetBlueprintLookup;
// This class does not need to be modified.
UINTERFACE()
class UCavrnusBaseUISystem : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class CAVRNUSCONNECTOR_API ICavrnusBaseUISystem
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void Initialize(UCavrnusWidgetBlueprintLookup* InLookup, ICavrnusWidgetDisplayer* InDisplayer) = 0;

	virtual void Close(UCavrnusBaseUserWidget* WidgetToClose) = 0;
	virtual void CloseAll() = 0;

	virtual void Teardown()=0;
};