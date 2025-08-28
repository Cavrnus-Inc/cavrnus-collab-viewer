// Copyright (c) 2024 Cavrnus. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "UI/CavrnusCVTEditorUIManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogCavrnusCVTEditor, Log, All);

class FUICommandList;

class FCavrnusCVTEditorModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	TStrongObjectPtr<UCavrnusCVTEditorUIManager> EditorUI;
};
