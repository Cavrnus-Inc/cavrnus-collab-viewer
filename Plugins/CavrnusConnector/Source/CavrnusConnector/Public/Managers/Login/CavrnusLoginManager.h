// // Copyright (c) 2025 Cavrnus. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "LoginFlows/CavrnusLoginBaseFlow.h"
#include "UObject/Object.h"
#include "CavrnusLoginManager.generated.h"

UCLASS()
class CAVRNUSCONNECTOR_API UCavrnusLoginManager : public UObject
{
	GENERATED_BODY()
public:
	void DoLogin(const FString& InLoginFlowType, const FCavrnusLoginConfig InConfig);
	void DoPluginSettingsLogin();
	void DoPieLogin();

	static void ResolveServer(FString& InServer);
	
private:
	bool HasAttemptedToLoginAlready = false;
	
	UPROPERTY()
	TObjectPtr<UCavrnusLoginBaseFlow> LoginFlow;
	
	bool ApplyCommandLineArgs(FCavrnusLoginConfig* InConfig);

};
