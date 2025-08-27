// Copyright (c) 2025 Cavrnus. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CavrnusCVTEditorUIManager.generated.h"

UCLASS()
class CAVRNUSCVTEDITOR_API UCavrnusCVTEditorUIManager : public UObject
{
	GENERATED_BODY()
public:
	void Initialize();
	void Teardown();
};
