// Copyright(c) Cavrnus. All rights reserved.

// Includes
#include "CavrnusConnectorModule.h"
#include "CavrnusConnectorSettings.h"
#include "CavrnusFunctionLibrary.h"
#include <Misc/Paths.h>

#include "CavrnusRelayLibrary.h"

#define LOCTEXT_NAMESPACE "CavrnusRelayModule"
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
}

//===============================================================
void FCavrnusConnectorModule::ShutdownModule()
{
}

#if PLATFORM_WINDOWS
#include "Windows/HideWindowsPlatformTypes.h"
#endif

#undef LOCTEXT_NAMESPACE
