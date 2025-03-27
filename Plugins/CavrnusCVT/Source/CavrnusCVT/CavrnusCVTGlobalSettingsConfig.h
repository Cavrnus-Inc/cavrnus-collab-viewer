// Copyright (c) 2024 Cavrnus. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include <Engine/DeveloperSettings.h>
#include "CavrnusCVTGlobalSettingsConfig.generated.h"

UCLASS(config = Game, defaultconfig, meta = (DisplayName = "Cavrnus CVT Settings"))
class CAVRNUSCVT_API UCavrnusCvtGlobalSettingsConfig : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, config, Category = "General Settings")
	bool bTextAnnotationSupportWithEpicGamesNDisplay;
};