// // Copyright (c) 2025 Cavrnus. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ListView.h"
#include "CavrnusListView.generated.h"

UCLASS()
class CAVRNUSCONNECTOR_API UCavrnusListView : public UListView
{
	GENERATED_BODY()
public:
	void SetEntryWidgetClass(const TSubclassOf<UUserWidget>& NewEntryWidgetClass);
	void AddItemAt(UObject* Item, int32 Index);
};
