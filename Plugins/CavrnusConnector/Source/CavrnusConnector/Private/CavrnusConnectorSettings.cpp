// Copyright (c) 2025 Cavrnus. All rights reserved.

#include "CavrnusConnectorSettings.h"
#include "Engine/Engine.h"
#include "UI/Helpers/CavrnusWidgetFactory.h"

//=====================================================================
UCavrnusConnectorSettings::UCavrnusConnectorSettings(const FObjectInitializer& obj)
{
	ConnectOnStart = true;
	SaveUserAuthToken = false;
	ServerDomain = "";

	AuthMethod = ECavrnusAuthMethod::JoinAsMember;
	GuestLoginMethod = ECavrnusGuestLoginMethod::PromptToEnterName;
	MemberLoginMethod = ECavrnusMemberLoginMethod::EnterMemberCredentials;
	GuestName = ""; // Guest ?
	MemberLoginEmail = "";
	MemberLoginPassword = ""; // Password ?
	SpaceJoinMethod = ECavrnusSpaceJoinMethod::SpacesListMenu;
	JoinId = "";
	OpenAiApiKey = "";
	RelayNetPort = 36045;
	RelayNetSilent = true;
	RelayNetVerboseLogging = false;
	RelayNetLogOutputToFile = false;
	RelayNetDisableRTC = false;

	// Apply defaults only if config did not provide them
	ServerSelectionMenu = FCavrnusWidgetFactory::GetDefaultBlueprint(TEXT("/CavrnusConnector/UI/Menus/ServerMenu/WBP_ServerSelectionMenu.WBP_ServerSelectionMenu_C"), UUserWidget::StaticClass());

	GuestJoinMenu = FCavrnusWidgetFactory::GetDefaultBlueprint(TEXT("/CavrnusConnector/UI/Menus/LoginMenus/WBP_GuestLogin.WBP_GuestLogin_C"), UUserWidget::StaticClass());

	MemberLoginMenu = FCavrnusWidgetFactory::GetDefaultBlueprint(TEXT("/CavrnusConnector/UI/Menus/LoginMenus/WBP_MemberLogin.WBP_MemberLogin_C"), UUserWidget::StaticClass());

	JoinIdMenu = FCavrnusWidgetFactory::GetDefaultBlueprint(TEXT("/CavrnusConnector/UI/Menus/LoginMenus/WBP_JoinIdLogin.WBP_JoinIdLogin_C"), UUserWidget::StaticClass());

	SpacesListMenu = FCavrnusWidgetFactory::GetDefaultBlueprint(TEXT("/CavrnusConnector/UI/Menus/SpaceListMenu/WBP_SpaceSelection.WBP_SpaceSelection_C"), UUserWidget::StaticClass());

	LoadingWidgetMenu = FCavrnusWidgetFactory::GetDefaultBlueprint(TEXT("/CavrnusConnector/UI/Menus/LoadingMenu/WBP_LoadingWidget.WBP_LoadingWidget_C"), UUserWidget::StaticClass());

	AuthenticationWidgetMenu = FCavrnusWidgetFactory::GetDefaultBlueprint(TEXT("/CavrnusConnector/UI/Menus/LoadingMenu/WBP_AuthenticationWidget.WBP_AuthenticationWidget_C"), UUserWidget::StaticClass());

	WidgetsToLoad.Add(FCavrnusWidgetFactory::GetDefaultBlueprint(TEXT("/CavrnusConnector/UI/Menus/MinimalUI/WBP_Cavrnus_MinimalUI.WBP_Cavrnus_MinimalUI_C"), UUserWidget::StaticClass()));
}

//=====================================================================
void UCavrnusConnectorSettings::PostInitProperties()
{
	Super::PostInitProperties();
	UE_LOG(LogTemp, Warning, TEXT("XXX Config name: %s"), *GetClass()->GetConfigName());
}


#if WITH_EDITOR
//=====================================================================
void UCavrnusConnectorSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	if (PropertyChangedEvent.ChangeType != EPropertyChangeType::Interactive)
		SaveConfig(CPF_Config, *GetClass()->GetConfigName());
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
