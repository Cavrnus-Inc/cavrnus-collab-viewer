// Copyright (c) 2024 Cavrnus. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FCavrnusCVTModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
