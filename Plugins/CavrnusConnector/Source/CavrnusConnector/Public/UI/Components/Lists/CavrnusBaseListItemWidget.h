// // Copyright (c) 2025 Cavrnus. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "UI/CavrnusBaseUserWidget.h"
#include "CavrnusBaseListItemWidget.generated.h"

/**
 * Use this widget for list items
 */
UCLASS(Abstract)
class CAVRNUSCONNECTOR_API UCavrnusBaseListItemWidget : public UCavrnusBaseUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
public:
	virtual void SetListIndex(int32 InIndex);
	virtual void SetSelectedState(const bool bState);
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

protected:
	void ListViewSetSelectedItem();
};
