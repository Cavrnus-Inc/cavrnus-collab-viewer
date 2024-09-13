// Copyright(c) Cavrnus. All rights reserved.
#include "CavrnusConnectorSettings.h"
#include "Engine/Engine.h"

//=====================================================================
UCavrnusConnectorSettings::UCavrnusConnectorSettings(const FObjectInitializer& obj)
{
	// Init default values
	RelayNetPort = 36045;
	RelayNetIPAddress = "127.0.0.1";
	RelayNetExecutableRelativeLocation = "CavrnusRelayNet\\net6.0-windows\\CavrnusRelayNet.exe";
	RelayNetVerboseLogging = false;
	AutoStartConnector = false;

	// Then load the config
	this->LoadConfig();

}

//=====================================================================
void UCavrnusConnectorSettings::PostInitProperties()
{
	Super::PostInitProperties();
}

#if WITH_EDITOR
//=====================================================================
void UCavrnusConnectorSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (PropertyChangedEvent.ChangeType != EPropertyChangeType::Interactive)
	{
		SaveConfig();
	}
}
#endif

FString UCavrnusConnectorSettings::GetRelayNetOptionalParameters() const
{
	FString result;

	if (RelayNetVerboseLogging)
	{
		result = "-v";
	}

	if (RelayNetLogOutputToFile)
	{
		result.Append(result.IsEmpty() ? "-f" : " -f");
	}

	if (RelayNetDisableRTC)
	{
		result.Append(result.IsEmpty() ? "-d" : " -d");
	}

	return result;
}
