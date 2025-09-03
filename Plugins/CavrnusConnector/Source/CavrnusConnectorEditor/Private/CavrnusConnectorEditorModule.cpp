// Copyright (c) 2025 Cavrnus. All rights reserved.

#include "CavrnusConnectorEditorModule.h"

#include "ToolMenus.h"
#include "Engine/World.h"
#include "Logging/LogMacros.h"

#define LOCTEXT_NAMESPACE "CavrnusConnectorEditor"

IMPLEMENT_MODULE(FCavrnusConnectorEditorModule, CavrnusConnectorEditor)
DEFINE_LOG_CATEGORY(LogCavrnusConnectorEditor);

FCavrnusConnectorEditorModule::FCavrnusConnectorEditorModule() {}
FCavrnusConnectorEditorModule::~FCavrnusConnectorEditorModule() {}

void FCavrnusConnectorEditorModule::StartupModule()
{
	IModuleInterface::StartupModule();
	
	EditorUI = TStrongObjectPtr(NewObject<UCavrnusEditorUIManager>(GetTransientPackage()));
	EditorUI->Initialize();

	// EditorTools = NewObject<UCavrnusEditorToolsManager>(GetTransientPackage());
	// EditorTools->AddToRoot();
	// EditorTools->Initialize();
}

void FCavrnusConnectorEditorModule::ShutdownModule()
{
	IModuleInterface::ShutdownModule();
	
	if (EditorUI.IsValid())		EditorUI->Teardown();
	if (EditorTools.IsValid())	EditorTools->Teardown();

	// Drop strong references; GC can collect afterward.
	EditorUI.Reset();
	EditorTools.Reset();
}

#undef LOCTEXT_NAMESPACE