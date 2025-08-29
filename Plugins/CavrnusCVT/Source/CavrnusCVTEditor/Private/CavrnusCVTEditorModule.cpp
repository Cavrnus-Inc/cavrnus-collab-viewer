// Copyright (c) 2024 Cavrnus. All rights reserved.

#include "CavrnusCVTEditorModule.h"

#include "LevelEditor.h"
#include "UI/Tabs/CavrnusEditorTabController.h"
#include "UI/Widgets/CavrnusMainEditorPanelWidget.h"

#define LOCTEXT_NAMESPACE "CavrnusCVTEditor"
IMPLEMENT_MODULE(FCavrnusCVTEditorModule, CavrnusCVTEditor)
DEFINE_LOG_CATEGORY(LogCavrnusCVTEditor);

void FCavrnusCVTEditorModule::StartupModule()
{
	IModuleInterface::StartupModule();

	EditorUI = TStrongObjectPtr(NewObject<UCavrnusCVTEditorUIManager>(GetTransientPackage()));
	EditorUI->Initialize();
}

void FCavrnusCVTEditorModule::ShutdownModule()
{
	IModuleInterface::ShutdownModule();

	if (EditorUI.IsValid())
		EditorUI->Teardown();
}