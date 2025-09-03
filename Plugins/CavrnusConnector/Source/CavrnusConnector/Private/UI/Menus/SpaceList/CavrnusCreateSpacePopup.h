// Copyright (c) 2025 Cavrnus. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/Menus/SpaceList/CavrnusSpaceListWidget.h"
#include "CavrnusCreateSpacePopup.generated.h"

class UEditableTextBox;

UCLASS()
class UCavrnusCreateSpacePopup : public UUserWidget
{
	GENERATED_BODY()

public:
	void Setup(const FSpaceSelectedEvent& OnSelectSpace);

	void SpaceCreated(const FCavrnusSpaceInfo& SpaceInfo);
};
