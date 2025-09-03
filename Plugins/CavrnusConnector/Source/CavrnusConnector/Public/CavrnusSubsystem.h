// // Copyright (c) 2025 Cavrnus. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Managers/CavrnusEditorAuthenticationManager.h"
#include "Managers/Login/CavrnusLoginManager.h"
#include "Subsystems/EngineSubsystem.h"
#include "CavrnusSubsystem.generated.h"

UCLASS()
class CAVRNUSCONNECTOR_API UCavrnusSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()
public:
	static UCavrnusSubsystem* Get();
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UCavrnusEditorAuthenticationManager* GetAuthManager() const
	{
		return CachedAuthManager;
	}
	
	UCavrnusLoginManager* GetLoginManager();
	
	UCavrnusConnectorSettings* GetSettings()
	{
#if WITH_EDITOR
		// Editor build: allow mutation
		if (UCavrnusConnectorSettings* FoundSettings = GetMutableDefault<UCavrnusConnectorSettings>())
		{
			return FoundSettings;
		}
#else
		// Runtime build: read-only access
		if (const UCavrnusConnectorSettings* FoundSettings = GetDefault<UCavrnusConnectorSettings>())
		{
			return const_cast<UCavrnusConnectorSettings*>(FoundSettings); // if caller expects non-const
		}
#endif
		UE_LOG(LogTemp, Error, TEXT("Unabled to find CavrnusSettings!"));
		return nullptr;
	}

private:
	FDelegateHandle WorldHandle = FDelegateHandle();
	
	UPROPERTY()
	TObjectPtr<UCavrnusLoginManager> CachedLoginManager = nullptr;
	
	UPROPERTY()
	TObjectPtr<UCavrnusEditorAuthenticationManager> CachedAuthManager = nullptr;
};
