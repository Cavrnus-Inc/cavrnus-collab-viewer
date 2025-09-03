// // Copyright (c) 2025 Cavrnus. All rights reserved.

#include "Managers/Login/CavrnusLoginManager.h"
#include "CavrnusConnectorModule.h"
#include "CavrnusFunctionLibrary.h"
#include "CavrnusSubsystem.h"
#include "Managers/Login/CavrnusLoginConfig.h"
#include "Managers/Login/LoginFlows/CavrnusPIELoginFlow.h"
#include "Managers/Login/LoginFlows/CavrnusRuntimeLoginFlow.h"
#include "Managers/Login/LoginFlows/CavrnusPluginSettingsLoginFlow.h"
#include "RelayModel/CavrnusRelayModel.h"
#include "UI/CavrnusUI.h"

void UCavrnusLoginManager::DoLogin(const FString& InLoginFlowType, FCavrnusLoginConfig InConfig)
{
	if (HasAttemptedToLoginAlready)
	{
		UE_LOG(LogCavrnusConnector, Warning, TEXT("Multiple login attempts detected before authentication completed."
											" Check Cavrnus Connector Plugin settings and set [ConnectOnStart] to false"))
	}
	
	const auto LoginFlowType = InLoginFlowType.TrimStartAndEnd().ToLower();

	if (LoginFlowType == TEXT("runtime"))
		LoginFlow = NewObject<UCavrnusRuntimeLoginFlow>();
	else if (LoginFlowType == TEXT("settings"))
		LoginFlow = NewObject<UCavrnusPluginSettingsLoginFlow>();
	else if (LoginFlowType == TEXT("pie"))
		LoginFlow = NewObject<UCavrnusPIELoginFlow>();
	else
	{
		UE_LOG(LogCavrnusConnector, Warning, TEXT("Unrecognized login flow: [%s] -- defaulting to DefaultLogin!"), *InLoginFlowType);
		LoginFlow = NewObject<UCavrnusRuntimeLoginFlow>();
	}
	
	ApplyCommandLineArgs(&InConfig);
	UCavrnusFunctionLibrary::SetupCavrnusEventHooks();

	ResolveServer(InConfig.Server);
	UCavrnusUI::Get()->AwaitUIInit([this, InConfig]
	{
		LoginFlow->DoLogin(InConfig);
	});

	HasAttemptedToLoginAlready = true;
}

void UCavrnusLoginManager::DoPluginSettingsLogin()
{
	auto* Settings = UCavrnusSubsystem::Get()->GetSettings();

	if (Settings == nullptr)
	{
		UE_LOG(LogCavrnusConnector, Error, TEXT("[UCavrnusLoginManager::DoPluginSettingsLogin] Unable to login! CavrnusConnectorSettings is null!"));
		return;
	}

	FCavrnusLoginConfig LoginConfig;
	LoginConfig.bIsPieUserLogin = false;
	LoginConfig.Server = Settings->ServerDomain;
	
	switch (Settings->AuthMethod)
	{
	case ECavrnusAuthMethod::Custom:
		LoginConfig.AuthMethod = ECavrnusAuthMethod::Custom;
		break;
	case ECavrnusAuthMethod::JoinAsMember:
		LoginConfig.AuthMethod = ECavrnusAuthMethod::JoinAsMember;
		LoginConfig.MemberLoginEmail = Settings->MemberLoginEmail;
		LoginConfig.MemberLoginPassword = Settings->MemberLoginPassword;
		LoginConfig.MemberLoginMethod = Settings->MemberLoginMethod;
		LoginConfig.SpaceJoinMethod = Settings->SpaceJoinMethod;
		LoginConfig.SpaceJoinId = Settings->JoinId;
		break;
	case ECavrnusAuthMethod::JoinAsGuest:
		LoginConfig.AuthMethod = ECavrnusAuthMethod::JoinAsGuest;
		LoginConfig.GuestName = Settings->GuestName;
		LoginConfig.GuestLoginMethod = Settings->GuestLoginMethod;
		LoginConfig.SpaceJoinMethod = Settings->SpaceJoinMethod;
		LoginConfig.SpaceJoinId = Settings->JoinId;
		break;
	}

	DoLogin("settings", LoginConfig);
}

void UCavrnusLoginManager::DoPieLogin()
{
	auto* Settings = UCavrnusSubsystem::Get()->GetSettings();

	if (Settings == nullptr)
	{
		UE_LOG(LogCavrnusConnector, Error, TEXT("[UCavrnusLoginManager::DoPieLogin] Unable to login! CavrnusConnectorSettings is null!"));
		return;
	}

	auto* AuthManager = UCavrnusSubsystem::Get()->GetAuthManager();
	if (AuthManager == nullptr)
	{
		UE_LOG(LogCavrnusConnector, Error, TEXT("[UCavrnusLoginManager::DoPieLogin] Unable to login! AuthManager is null!"));
		return;
	}
	
	FCavrnusLoginConfig LoginConfig;
	LoginConfig.Server = AuthManager->GetPIEAuthedServer();

	FCavrnusEditorLoginInfo EditorLogin;
	
	switch (AuthManager->GetCurrentAuthMethod())
	{
	case ECavrnusAuthMethodForPIE::JoinAsPIE:
		LoginConfig.bIsPieUserLogin = true;
		if (UCavrnusSubsystem::Get()->GetAuthManager()->TryGetEditorLoginInfo(EditorLogin))
		{
			LoginConfig.ApiKey = EditorLogin.AccessKey;
			LoginConfig.ApiToken = EditorLogin.AccessToken;
		}
		break;
	case ECavrnusAuthMethodForPIE::JoinAsMember:
		LoginConfig.AuthMethod = ECavrnusAuthMethod::JoinAsMember;
		break;
	case ECavrnusAuthMethodForPIE::JoinAsGuest:
		LoginConfig.AuthMethod = ECavrnusAuthMethod::JoinAsGuest;
		break;
	}
	
	DoLogin("pie", LoginConfig);
}

bool UCavrnusLoginManager::ApplyCommandLineArgs(FCavrnusLoginConfig* InConfig)
{
	auto Overridden = false;
	
	FString Server;
	if (FParse::Value(FCommandLine::Get(), TEXT("Server="), Server))
	{
		InConfig->Server = Server;
		Overridden = true;
	}

	FString GuestName;
	if (FParse::Value(FCommandLine::Get(), TEXT("GuestName="), GuestName))
	{
		InConfig->AuthMethod = ECavrnusAuthMethod::JoinAsGuest;
		InConfig->GuestName = GuestName;
		Overridden = true;
	}

	FString UserEmail;
	if (FParse::Value(FCommandLine::Get(), TEXT("UserEmail="), UserEmail))
	{
		InConfig->AuthMethod = ECavrnusAuthMethod::JoinAsMember;
		InConfig->MemberLoginEmail = UserEmail;
		Overridden = true;
	}
	FString UserPassword;
	if (FParse::Value(FCommandLine::Get(), TEXT("UserPassword="), UserPassword))
	{
		InConfig->AuthMethod = ECavrnusAuthMethod::JoinAsMember;
		InConfig->MemberLoginPassword = UserPassword;
		Overridden = true;
	}

	FString SpaceJoinId;
	if (FParse::Value(FCommandLine::Get(), TEXT("SpaceJoinId="), SpaceJoinId))
	{
		InConfig->SpaceJoinId = SpaceJoinId;
		Overridden = true;
	}

	return Overridden;
}

void UCavrnusLoginManager::ResolveServer(FString& InServer)
{
	// Trim whitespace
	InServer.TrimStartAndEndInline();

	// If empty after trimming, just bail
	if (InServer.IsEmpty())
	{
		InServer = TEXT("");
		return;
	}

	// Strip http:// or https://
	if (InServer.StartsWith(TEXT("http://"), ESearchCase::IgnoreCase))
		InServer.RightChopInline(7);
	else if (InServer.StartsWith(TEXT("https://"), ESearchCase::IgnoreCase))
		InServer.RightChopInline(8);

	// Trim trailing slashes
	while (InServer.EndsWith(TEXT("/")))
		InServer.LeftChopInline(1);

	// Ensure it ends with ".cavrn.us"
	if (!InServer.Contains(TEXT(".cavrn.us"), ESearchCase::IgnoreCase))
	{
		// Trim trailing dots first
		while (InServer.EndsWith(TEXT(".")))
			InServer.LeftChopInline(1);

		InServer += TEXT(".cavrn.us");
	}
	
	// Final cleanup: remove trailing dots just in case
	while (InServer.EndsWith(TEXT(".")))
		InServer.LeftChopInline(1);

	// Lowercase everything
	InServer = InServer.ToLower();
}
