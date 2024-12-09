// Copyright (c) 2024 Cavrnus. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CavrnusConnectorSettings.generated.h"

UCLASS(Config=Cavrnus)
class CAVRNUSCONNECTOR_API UCavrnusConnectorSettings : public UObject
{
	GENERATED_BODY()

public:

	// Functions

	// Constructor
	UCavrnusConnectorSettings(const FObjectInitializer& obj);

	// Initialization
	virtual void PostInitProperties() override;

	// Generates the string containing the verbose parameter list for CavrnusRelayNet
	FString GetRelayNetOptionalParameters() const;

#if WITH_EDITOR
	// Called when an edit is made to the settings
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	// Variables

	// Should we automatically try to launch CavrnusRelayNet and connect to it?
	UPROPERTY(Config, EditAnywhere, Category = "Configuration")
	bool AutoStartConnector;

	// Port that the relay will use when launching the relay net .exe
	UPROPERTY(Config, EditAnywhere, Category = "Configuration")
	int RelayNetPort;

	// IP address that the relay will use when launching the relay net .exe
	UPROPERTY(Config, EditAnywhere, Category = "Configuration")
	FString RelayNetIPAddress;

	// Relative location of the RelayNet binaries from the plugin root dir
	UPROPERTY(Config, EditAnywhere, Category = "Configuration")
	FString RelayNetRelativeLocation;

	// Prevents the console window from being visible to users
	UPROPERTY(Config, EditAnywhere, Category = "Configuration")
	bool RelayNetSilent;

	// Optional Parameters:
	//   "-v" or "-V": Verbose logging in the CavrnusRelayNet process
	UPROPERTY(Config, EditAnywhere, Category = "Configuration")
	bool RelayNetVerboseLogging;

	//   "-f" or "-F": Logging in the CavrnusRelayNet process saved to a file
	UPROPERTY(Config, EditAnywhere, Category = "Configuration")
	bool RelayNetLogOutputToFile;

	//   "-d" or "-D": Disables voice and video communication
	UPROPERTY(Config, EditAnywhere, Category = "Configuration")
	bool RelayNetDisableRTC;
};
