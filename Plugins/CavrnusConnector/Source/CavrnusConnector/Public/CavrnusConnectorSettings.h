// Copyright (c) 2025 Cavrnus. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ISettingsModule.h"
#include "UObject/Object.h"
#include "Blueprint/UserWidget.h"
#include "Modules/ModuleManager.h"
#include "Modules/ModuleInterface.h"
#include "CavrnusConnectorSettings.generated.h"

UENUM(BlueprintType)
enum class ECavrnusAuthMethod : uint8
{
	Custom,
	JoinAsMember,
	JoinAsGuest
};

UENUM(BlueprintType)
enum class ECavrnusAuthMethodForPIE : uint8
{
	JoinAsPIE UMETA(DisplayName = "JoinAsPIE"),
	JoinAsMember UMETA(DisplayName = "JoinAsMember"),
	JoinAsGuest  UMETA(DisplayName = "JoinAsGuest")
};

UENUM(BlueprintType)
enum class ECavrnusMemberLoginMethod : uint8
{
	Custom,
	EnterMemberCredentials,
	PromptMemberToLogin
};

UENUM(BlueprintType)
enum class ECavrnusGuestLoginMethod : uint8
{
	Custom,
	EnterNameBelow,
	PromptToEnterName
};

UENUM(BlueprintType)
enum class ECavrnusSpaceJoinMethod : uint8
{
	Custom,
	EnterJoinId,
	SpacesListMenu,
	PromptUserForJoinId,
};

UCLASS(config=Cavrnus, defaultconfig)
class CAVRNUSCONNECTOR_API UCavrnusConnectorSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	static void Show()
	{
		if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
			SettingsModule->ShowViewer("Project", "Plugins", "CavrnusSpatialConnector");
	}
	
#if WITH_EDITOR
	virtual FName GetCategoryName() const override
	{
		return TEXT("Plugins"); // Top-level category
	}
	virtual FName GetSectionName() const override
	{
		return TEXT("CavrnusSpatialConnector"); // Subsection label
	}
#endif

	UCavrnusConnectorSettings(const FObjectInitializer& obj);
	
	virtual void PostInitProperties() override;

	FString GetRelayNetOptionalParameters() const;

#if WITH_EDITOR
	// Called when an edit is made to the settings
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	// Automatically connect on Start.  Disable if you manually connect via Blueprint or C++
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Sign-in Options")
	bool ConnectOnStart = true;

	// Locally cache the User Authentication Token after successful login
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Sign-in Options")
	bool SaveUserAuthToken = false;

	// Hard-code the Server Domain. If blank, User will be prompted to enter it
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Sign-in Options", meta = (EditCondition = "AuthMethod != ECavrnusAuthMethod::Custom", EditConditionHides))
	FString ServerDomain = "";

	// Select User Authentication method
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Sign-in Options", meta = (DisplayName = "Authentication Method"))
	ECavrnusAuthMethod AuthMethod = ECavrnusAuthMethod::JoinAsMember;

	// Select the method for defining the Guest User name  
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Sign-in Options|Guest", meta = (EditCondition = "AuthMethod == ECavrnusAuthMethod::JoinAsGuest", EditConditionHides))
	ECavrnusGuestLoginMethod GuestLoginMethod = ECavrnusGuestLoginMethod::PromptToEnterName;

	// Hard-code the Name that a Guest User will have
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Sign-in Options|Guest", meta = (EditCondition = "AuthMethod == ECavrnusAuthMethod::JoinAsGuest && GuestLoginMethod == ECavrnusGuestLoginMethod::EnterNameBelow", EditConditionHides))
	FString GuestName = "";

	// Select the method for obtaining the Team Member authentication credentials
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Sign-in Options|Member", meta = (EditCondition = "AuthMethod == ECavrnusAuthMethod::JoinAsMember", EditConditionHides))
	ECavrnusMemberLoginMethod MemberLoginMethod = ECavrnusMemberLoginMethod::EnterMemberCredentials;

	// Hard-code the Team Member Email address
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Sign-in Options|Member", meta = (EditCondition = "AuthMethod == ECavrnusAuthMethod::JoinAsMember && MemberLoginMethod == ECavrnusMemberLoginMethod::EnterMemberCredentials", EditConditionHides))
	FString MemberLoginEmail = "";

	// Hard-code the Team Member Password
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Sign-in Options|Member", meta = (EditCondition = "AuthMethod == ECavrnusAuthMethod::JoinAsMember && MemberLoginMethod == ECavrnusMemberLoginMethod::EnterMemberCredentials", EditConditionHides, PasswordField = true))
	FString MemberLoginPassword = "";

	// Select the method for choosing which Space to join
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Sign-in Options|Space", meta = (EditCondition = "AuthMethod != ECavrnusAuthMethod::Custom", EditConditionHides))
	ECavrnusSpaceJoinMethod SpaceJoinMethod = ECavrnusSpaceJoinMethod::SpacesListMenu;
	
	// Hard-code the Join ID for a Space
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Sign-in Options|Space", meta = (EditCondition = "AuthMethod != ECavrnusAuthMethod::Custom && SpaceJoinMethod == ECavrnusSpaceJoinMethod::EnterJoinId", EditConditionHides))
	FString JoinId = "";

	// Enter your OpenAI API Key to use with the Platform's Extensions
	UPROPERTY(Config, EditAnywhere, Category = "AiIntegration")
	FString OpenAiApiKey;

	// Select the Widget Blueprint for the Server Domain entry
	UPROPERTY(Config, EditAnywhere, Category = "Menu Widgets")
	TSubclassOf<UUserWidget> ServerSelectionMenu;

	// Select the Widget Blueprint for the Guest Name entry
	UPROPERTY(Config, EditAnywhere, Category = "Menu Widgets")
	TSubclassOf<UUserWidget> GuestJoinMenu;

	// Select the Widget Blueprint for the Team Member login
	UPROPERTY(Config, EditAnywhere, Category = "Menu Widgets")
	TSubclassOf<UUserWidget> MemberLoginMenu;

	// Select the Widget Blueprint for the Space List Menu
	UPROPERTY(Config, EditAnywhere, Category = "Menu Widgets")
	TSubclassOf<UUserWidget> SpacesListMenu;

	// Select the Widget Blueprint for entering the Join ID for a Space
	UPROPERTY(Config, EditAnywhere, Category = "Menu Widgets")
	TSubclassOf<UUserWidget> JoinIdMenu;

	UPROPERTY(Config, EditAnywhere, Category = "Menu Widgets")
	TSubclassOf<UUserWidget> AuthenticationWidgetMenu;

	UPROPERTY(Config, EditAnywhere, Category = "Menu Widgets")
	TSubclassOf<UUserWidget> LoadingWidgetMenu;

	UPROPERTY(Config, EditAnywhere, Category = "Menu Widgets")
	TArray<TSubclassOf<UUserWidget>> WidgetsToLoad;

	// Port that the relay will use when launching the CollaborationCommunicationSystem process
	UPROPERTY(Config, EditAnywhere, Category = "Relay Net Configuration")
	int RelayNetPort = 36045;

	// Prevents the CollaborationCommunicationSystem console window from being visible to users
	UPROPERTY(Config, EditAnywhere, Category = "Relay Net Configuration")
	bool RelayNetSilent = true;

	//   "-v" or "-V": Verbose logging in the CollaborationCommunicationSystem process
	UPROPERTY(Config, EditAnywhere, Category = "Relay Net Configuration")
	bool RelayNetVerboseLogging = false;

	//   "-f" or "-F": Logging in the CollaborationCommunicationSystem process saved to a file
	UPROPERTY(Config, EditAnywhere, Category = "Relay Net Configuration")
	bool RelayNetLogOutputToFile = false;

	//   "-d" or "-D": Disables voice and video communication
	UPROPERTY(Config, EditAnywhere, Category = "Relay Net Configuration")
	bool RelayNetDisableRTC = false;
};
