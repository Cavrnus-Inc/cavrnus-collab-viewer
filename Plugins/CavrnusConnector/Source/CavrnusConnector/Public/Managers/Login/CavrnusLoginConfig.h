// // Copyright (c) 2025 Cavrnus. All rights reserved.

#pragma once
#include "CavrnusConnectorSettings.h"

struct FCavrnusLoginConfig
{
	bool bIsPieUserLogin = false;
	
	ECavrnusAuthMethod AuthMethod = ECavrnusAuthMethod::Custom;
	
	ECavrnusMemberLoginMethod MemberLoginMethod = ECavrnusMemberLoginMethod::Custom;
	ECavrnusGuestLoginMethod GuestLoginMethod = ECavrnusGuestLoginMethod::Custom;
	ECavrnusSpaceJoinMethod SpaceJoinMethod = ECavrnusSpaceJoinMethod::Custom;
	
	FString Server = "";
	FString SpaceJoinId = "";

	FString ApiToken = "";
	FString ApiKey = "";
	 
	FString GuestName = "";
	
	FString MemberLoginEmail = "";
	FString MemberLoginPassword = "";
};
