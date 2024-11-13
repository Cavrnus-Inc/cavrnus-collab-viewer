// Fill out your copyright notice in the Description page of Project Settings.

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