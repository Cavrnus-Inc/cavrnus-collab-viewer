#pragma once

#include "CoreMinimal.h"
#include "CavrnusSpatialConnector.h"
#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogCavrnusCVTEditor, Log, All);

class FUICommandList;

class FCavrnusCVTEditorModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	TSharedPtr<FUICommandList> PluginCommands;

	void RegisterMenus();
	void CreateCavrnusCvtRibbon(FMenuBarBuilder& Builder);
	void CreateRibbonSubEntry(FMenuBuilder& MenuBuilder);

	void ConvertStaticMeshActors();

	void SetupLevel();
	void SetGameMode();
	void TryAddManager();
	void TryAddSpatialConnector();
	
	void AddSpawnableItem(ACavrnusSpatialConnector& SC, const FString& Id, const FString& Path);
	UClass* GetDefaultBlueprint(const FString& Path, UClass* BaseClass);
};
