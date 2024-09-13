// Copyright(c) Cavrnus. All rights reserved.
#include "CavrnusSplashScreenWidget.h"
#include "Components/Button.h"
#include "Components/CheckBox.h"
#include "CavrnusConnectorEditorModule.h"
#include <EditorUtilitySubsystem.h>

void UCavrnusSplashScreenWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	FCavrnusConnectorEditorModule& CavrnusConnectorEditorModule =
		FModuleManager::LoadModuleChecked<FCavrnusConnectorEditorModule>
		(FName(TEXT("CavrnusConnectorEditor")));
	ShowOnStartupCheckbox->SetIsChecked(CavrnusConnectorEditorModule.ShouldShowSplashScreen());

	FPlatformMisc::GetStoredValue(TEXT("Cavrnus"), TEXT("UE"), TEXT("SavedServerName"), CustomerServer);
	ServerInputField->SetText(FText::FromString(CustomerServer));

	ServerInputField->OnTextChanged.AddDynamic(this, &UCavrnusSplashScreenWidget::OnServerInputFieldChanged);

	SetupSpaceButton->OnClicked.AddUniqueDynamic(this, &UCavrnusSplashScreenWidget::OnSetupSpaceClicked);
	OpenWebConsoleButton->OnClicked.AddUniqueDynamic(this, &UCavrnusSplashScreenWidget::OnWebConsoleClicked);
	OpenDocumentationButton->OnClicked.AddUniqueDynamic(this, &UCavrnusSplashScreenWidget::OnOpenDocsClicked);
	ShowOnStartupCheckbox->OnCheckStateChanged.AddUniqueDynamic(this, &UCavrnusSplashScreenWidget::OnShowSplashScreenOnStartupClicked);
	DismissButton->OnClicked.AddUniqueDynamic(this, &UCavrnusSplashScreenWidget::OnDismissClicked);
	ButtonJoinDiscord->OnClicked.AddUniqueDynamic(this, &UCavrnusSplashScreenWidget::OnJoinDiscordClicked);
}

void UCavrnusSplashScreenWidget::OnSetupSpaceClicked()
{
	FCavrnusConnectorEditorModule& CavrnusConnectorEditorModule =
		FModuleManager::LoadModuleChecked<FCavrnusConnectorEditorModule>
		(FName(TEXT("CavrnusConnectorEditor")));
	CavrnusConnectorEditorModule.AddCavrnusSpatialConnectorToLevel();
	Close();
}

void UCavrnusSplashScreenWidget::OnWebConsoleClicked()
{
	FPlatformProcess::LaunchURL(*WebConsoleURL, nullptr, nullptr);
}

void UCavrnusSplashScreenWidget::OnOpenDocsClicked()
{
	FPlatformProcess::LaunchURL(*DocumentationURL, nullptr, nullptr);
}

void UCavrnusSplashScreenWidget::OnJoinDiscordClicked()
{
	FPlatformProcess::LaunchURL(*DiscordURL, nullptr, nullptr);
}

void UCavrnusSplashScreenWidget::OnShowSplashScreenOnStartupClicked(bool bShowOnStartup)
{
	FCavrnusConnectorEditorModule& CavrnusConnectorEditorModule =
		FModuleManager::LoadModuleChecked<FCavrnusConnectorEditorModule>
		(FName(TEXT("CavrnusConnectorEditor")));
	CavrnusConnectorEditorModule.SetShowSplashScreen(bShowOnStartup);
}

void UCavrnusSplashScreenWidget::OnDismissClicked()
{
	Close();
}

void UCavrnusSplashScreenWidget::OnServerInputFieldChanged(const FText& ServerVal)
{
	const FString ServerValString = ServerVal.ToString();
	
	if (ServerValString != CustomerServer && !ServerValString.StartsWith(".") && !ServerValString.EndsWith("."))
	{
		FPlatformMisc::SetStoredValue(TEXT("Cavrnus"), TEXT("UE"), TEXT("SavedServerName"), ServerValString);
	}

	CustomerServer = ServerValString;
}

void UCavrnusSplashScreenWidget::Close()
{
	UEditorUtilitySubsystem* EditorUtilitySubsystem = GEditor->GetEditorSubsystem<UEditorUtilitySubsystem>();
	if (EditorUtilitySubsystem)
	{
		EditorUtilitySubsystem->CloseTabByID(TabID);
	}
}