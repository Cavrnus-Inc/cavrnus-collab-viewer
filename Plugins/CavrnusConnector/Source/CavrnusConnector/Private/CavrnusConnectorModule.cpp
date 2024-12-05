// Copyright (c) 2024 Cavrnus. All rights reserved.

// Includes
#include "CavrnusConnectorModule.h"
#include "CavrnusConnectorSettings.h"
#include "CavrnusFunctionLibrary.h"
#include <Misc/Paths.h>

#include "CavrnusRelayLibrary.h"

#if WITH_EDITOR
#include "ISettingsModule.h"
#include "ISettingsSection.h"
#endif

#define LOCTEXT_NAMESPACE "CavrnusConnectorModule"
DEFINE_LOG_CATEGORY(LogCavrnusConnector);
IMPLEMENT_MODULE(FCavrnusConnectorModule, CavrnusConnector)

//===============================================================
FCavrnusConnectorModule::FCavrnusConnectorModule()
{
}

//===============================================================
FCavrnusConnectorModule::~FCavrnusConnectorModule()
{
}


//===============================================================
void FCavrnusConnectorModule::StartupModule()
{
#if WITH_EDITOR
	// register settings
	ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");

	if (SettingsModule != nullptr)
	{
		ISettingsSectionPtr SettingsSection = SettingsModule->RegisterSettings("Project", "Plugins", "CavrnusConnector",
			LOCTEXT("CavrnusConnectorName", "Cavrnus Spatial Connector"),
			LOCTEXT("CavrnusConnectorDescription", "Configure the Cavrnus Spatial Connector plug-in."),
			GetMutableDefault<UCavrnusConnectorSettings>()
		);
	}
#endif //WITH_EDITOR
}

//===============================================================
void FCavrnusConnectorModule::ShutdownModule()
{
#if WITH_EDITOR
	// unregister settings
	ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");

	if (SettingsModule != nullptr)
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", "CavrnusConnector");
	}
#endif //WITH_EDITOR
}

#if PLATFORM_WINDOWS
#include "Windows/HideWindowsPlatformTypes.h"
#endif

#undef LOCTEXT_NAMESPACE
