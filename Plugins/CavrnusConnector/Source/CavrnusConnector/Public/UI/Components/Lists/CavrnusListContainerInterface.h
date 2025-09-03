// // Copyright (c) 2025 Cavrnus. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CavrnusBaseListItemWidget.h"
#include "UObject/Interface.h"
#include "CavrnusListContainerInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UCavrnusListContainerInterface : public UInterface
{
	GENERATED_BODY()
};
DECLARE_MULTICAST_DELEGATE_TwoParams(FCavrnusListItemGenerated, UUserWidget* /* CreatedWidget */, UObject* /* BoundObject */);
DECLARE_MULTICAST_DELEGATE_OneParam(FCavrnusListItemSelected, UObject* /* SelectedItem */);
class CAVRNUSCONNECTOR_API ICavrnusListContainerInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void InitializeList(TSubclassOf<UCavrnusBaseListItemWidget> EntryWidgetClass)= 0;
	virtual void ClearItems()= 0;
	
	virtual void AddItemAt(UObject* Item, int32 Index)= 0;
	virtual void AddItem(UObject* Item)= 0;
	virtual void RemoveItem(UObject* Item)= 0;
	virtual void SetSelectedItem(UObject* Item)= 0;
	virtual void TeardownList()=0;

	/** Optional: Expose the selection delegate directly */
	virtual FCavrnusListItemSelected& OnItemSelected() = 0;
	/** Optional: Expose the widget generated delegate */
	virtual FCavrnusListItemGenerated& OnEntryGenerated() = 0;
};
