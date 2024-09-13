// Copyright(c) Cavrnus. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogCavrnusConnectorEditor, Log, All);

class USCS_Node;
class UCavrnusValueSyncBase;

class FCavrnusConnectorEditorModule : public IModuleInterface
{
	private:
		// Used to store the SpawnableIdentifiers until an instance
		// of CavrnusSpatialConnector is created
		TMap<FString, TSubclassOf<AActor>> SpawnableIdentifiers;

		FDelegateHandle ShowSplashScreenDelegateHandle;
		FDelegateHandle BeginPIEDelegateHandle;
		FDelegateHandle EnginePreExitDelegateHandle;
		FDelegateHandle PostEngineInitDelegateHandle;
		FDelegateHandle MainFrameCreatedDelegateHandle;

		FName SplashScreenTabID;

		TSharedPtr<class FCavrnusTabStyle> EditorModeTabStyle;
	public:

		// Called when module is loaded in memory
		FCavrnusConnectorEditorModule();
		~FCavrnusConnectorEditorModule() override;

		/** IModuleInterface implementation */
		// Called when module is ready to start
		virtual void StartupModule() override;
		virtual void ShutdownModule() override;

		void RegisterStartupTab();
		void UnregisterStartupTab();
		
		void HookAutoAddContainerComponents();
		void UnhookAutoAddContainerComponents();

		void OnAssetOpened(UObject* Asset, IAssetEditorInstance* EditorInstance);
		void OnAssetRequestClose(UObject* Asset, EAssetEditorCloseReason CloseReason);

		FString GenerateContainerName(USCS_Node* ContainerParentNode, UBlueprint* Blueprint) const;
		FString GenerateContainerName(USceneComponent* ContainerParentNode, UBlueprint* Blueprint) const;

		void OnBlueprintChanged(UBlueprint* Blueprint);

		void DrawAttentionToSplashScreen();
		void CloseSplashWindow();

		void SetShowSplashScreen(bool bShowOnStartup);
		bool ShouldShowSplashScreen() const;

		void AddTopRibbonMenu(FMenuBarBuilder& MenuBuilder);
		void AddCavrnusToolbarEntry(FMenuBuilder& MenuBuilder);

		void SetWelcomeScreenState(bool bToShow);

		void RegisterToolsMenus();
		void AddCavrnusToolsOptions(FToolMenuSection& InSection);

		void AddCavrnusSpatialConnectorToLevel();

		/**
		* Returns root path for the this plugin.
		*/
		FString GetPluginPath();
private:
	UClass* GetDefaultBlueprint(const FString& Path, UClass* BaseClass) const;
};