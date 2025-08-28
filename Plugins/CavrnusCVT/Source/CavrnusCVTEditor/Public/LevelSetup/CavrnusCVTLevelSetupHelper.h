// Copyright (c) 2025 Cavrnus. All rights reserved.

#pragma once

#include "CoreMinimal.h"

class CAVRNUSCVTEDITOR_API FCavrnusCVTLevelSetupHelper
{
public:
	static void AddCollisionProfiles();
	static bool HasSetupCollisionProfiles();
	
	static void ConvertStaticMeshActors();
	
	static void AddCVTManagerToScene();
	static bool HasCvtManager();
	
	static void SetupGameMode();
	static bool HasGameMode();
private:
	static TArray<FString> CollisionProfiles;
	static void RegisterCollisionChannelIfMissing(const FString& ChannelName);
	
	static FString GetLevelGUID();
};
