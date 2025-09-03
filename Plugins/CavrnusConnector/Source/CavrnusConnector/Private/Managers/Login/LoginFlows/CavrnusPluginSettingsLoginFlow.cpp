// // Copyright (c) 2025 Cavrnus. All rights reserved.

#include "Managers/Login/LoginFlows/CavrnusPluginSettingsLoginFlow.h"
#include "CavrnusConnectorModule.h"
#include "CavrnusFunctionLibrary.h"
#include "CavrnusSubsystem.h"

void UCavrnusPluginSettingsLoginFlow::DoLogin(const FCavrnusLoginConfig& InLoginConfig)
{
	if (InLoginConfig.AuthMethod == ECavrnusAuthMethod::Custom)
	{
		UE_LOG(LogCavrnusConnector, Warning, TEXT("ECavrnusAuthMethod is set to custom! Default Cavrnus login flows will not execute."))
		return;
	}
	
	Super::DoLogin(InLoginConfig);
	AwaitValidServer([this, InLoginConfig]
	{
		if (InLoginConfig.AuthMethod == ECavrnusAuthMethod::JoinAsGuest)
			HandleGuestFlow();
		if (InLoginConfig.AuthMethod == ECavrnusAuthMethod::JoinAsMember)
			HandleMemberFlow();
	});
}

void UCavrnusPluginSettingsLoginFlow::HandleMemberFlow()
{
	UCavrnusFunctionLibrary::AwaitAuthentication([this](const FCavrnusAuthentication&)
	{
		if (LoginConfig.SpaceJoinMethod == ECavrnusSpaceJoinMethod::Custom)
		{
			UE_LOG(LogCavrnusConnector, Warning, TEXT("ECavrnusSpaceJoinMethod is set to custom! You are authenticated, but Cavrnus space joining will not occur."))
			return;
		}
		
		if (LoginConfig.SpaceJoinMethod == ECavrnusSpaceJoinMethod::EnterJoinId && !LoginConfig.SpaceJoinId.IsEmpty())
		{
			TryJoinSpace([]
			{
					
			}, [this](const FString&)
			{
				ShowSpaceListWidget();
			});
		}
		else if (LoginConfig.SpaceJoinMethod == ECavrnusSpaceJoinMethod::SpacesListMenu)
			ShowSpaceListWidget();
		else if (LoginConfig.SpaceJoinMethod == ECavrnusSpaceJoinMethod::PromptUserForJoinId)
			ShowJoinIdWidget();
	});

	if (!UCavrnusSubsystem::Get()->GetSettings()->SaveUserAuthToken)
		UCavrnusSubsystem::Get()->GetAuthManager()->SetRuntimeToken("");

	if (LoginConfig.MemberLoginMethod == ECavrnusMemberLoginMethod::EnterMemberCredentials)
	{
		if (UCavrnusSubsystem::Get()->GetSettings()->SaveUserAuthToken)
			TryMemberAuthWithRuntimeToken([] {  }, [this](const FString& ) { PromptMemberLoginAndSaveToken(); });
		else if (!LoginConfig.MemberLoginEmail.IsEmpty() && !LoginConfig.MemberLoginPassword.IsEmpty())
			TryMemberAuthWithPassword([] {  }, [this](const FString& ) { PromptMemberLogin(); });
		else
			PromptMemberLogin(); 
	}
	else if (LoginConfig.MemberLoginMethod == ECavrnusMemberLoginMethod::PromptMemberToLogin)
		PromptMemberLogin();
}

void UCavrnusPluginSettingsLoginFlow::HandleGuestFlow()
{
	UCavrnusFunctionLibrary::AwaitAuthentication([this](const FCavrnusAuthentication&)
	{
		if (LoginConfig.SpaceJoinMethod == ECavrnusSpaceJoinMethod::Custom)
		{
			UE_LOG(LogCavrnusConnector, Warning, TEXT("ECavrnusSpaceJoinMethod is set to custom! You are authenticated, but Cavrnus space joining will not occur."))
			return;
		}
		
		if (LoginConfig.SpaceJoinMethod == ECavrnusSpaceJoinMethod::PromptUserForJoinId)
			ShowJoinIdWidget();
		else if (LoginConfig.SpaceJoinMethod == ECavrnusSpaceJoinMethod::EnterJoinId && !LoginConfig.SpaceJoinId.IsEmpty())
		{
			TryJoinSpace([]
			{
					
			}, [this](const FString&)
			{
				ShowJoinIdWidget();
			});
		}
		else
			ShowJoinIdWidget();
	});

	if (LoginConfig.GuestLoginMethod == ECavrnusGuestLoginMethod::PromptToEnterName)
		PromptGuestLogin();
	else if (LoginConfig.GuestLoginMethod == ECavrnusGuestLoginMethod::EnterNameBelow)
	{
		if (LoginConfig.GuestName.IsEmpty())
			PromptGuestLogin();
		else
		{
			TryGuestAuth([] {  }, [this](const FString&)
			{
				PromptGuestLogin();
			});
		}
	}
}
