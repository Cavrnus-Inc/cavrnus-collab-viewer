// Copyright (c) 2024 Cavrnus. All rights reserved.

#include "CavrnusCVTEditorModule.h"

#include <CavrnusFunctionLibrary.h>

#include "CavrnusCVTManager.h"
#include "EngineUtils.h"
#include "LevelEditor.h"
#include "Engine/StaticMeshActor.h"
#include "GameFramework/GameModeBase.h"
#include "CavrnusConnectorSettings.h"

#include "UI/ServerSelectionMenu/CavrnusServerSelectWidget.h"

#define LOCTEXT_NAMESPACE "CavrnusCVTEditor"
IMPLEMENT_MODULE(FCavrnusCVTEditorModule, CavrnusCVTEditor)
DEFINE_LOG_CATEGORY(LogCavrnusCVTEditor);

void FCavrnusCVTEditorModule::StartupModule()
{
	IModuleInterface::StartupModule();

	RegisterMenus();
}

void FCavrnusCVTEditorModule::ShutdownModule()
{
	IModuleInterface::ShutdownModule();
}

void FCavrnusCVTEditorModule::RegisterMenus()
{
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");

	TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());

	MenuExtender->AddMenuBarExtension(
		"Help",
		EExtensionHook::After,
		nullptr,
		FMenuBarExtensionDelegate::CreateRaw(this, &FCavrnusCVTEditorModule::CreateCavrnusCvtRibbon)
	);

	LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);
}

void FCavrnusCVTEditorModule::CreateCavrnusCvtRibbon(FMenuBarBuilder& Builder)
{
	Builder.AddPullDownMenu(
		  LOCTEXT("MenuLocKey", "Cavrnus Collab Viewer"),
		  LOCTEXT("MenuTooltipKey", "Opens menu for CollabViewer plugin"),
		  FNewMenuDelegate::CreateRaw(this, &FCavrnusCVTEditorModule::CreateRibbonSubEntry),
		  FName(TEXT("Cavrnus")),
		  FName(TEXT("CavrnusHelpMenu"))
	  );
}
void FCavrnusCVTEditorModule::CreateRibbonSubEntry(FMenuBuilder& MenuBuilder)
{
	MenuBuilder.AddMenuEntry(
	LOCTEXT("SetupProject", "Setup Project Settings"),
	LOCTEXT("SetupProjectTooltip", "Adds missing collision profiles."),
	FSlateIcon(),
	FUIAction(FExecuteAction::CreateLambda([this]
	{
		const FText Title = FText::FromString("Confirm Project Settings Update");
		const FText Message = FText::FromString(
			TEXT("This will add missing collision profiles to the DefaultEngine.ini file.\n\n")
			TEXT("Unreal Editor will restart automatically afterwards. \n\nProceed?")
		);
		if (FMessageDialog::Open(EAppMsgType::YesNo, Message, Title))
		{
			AddCollisionProfiles();
			FUnrealEdMisc::Get().RestartEditor(true); // true = prompt to save
		}
	}))
	);
	
	MenuBuilder.AddMenuEntry(
		LOCTEXT("SetupLevel", "Setup level for Cavrnus Collab Viewer"),
		LOCTEXT("SetupLevelTooltip", "Configures SpatialConnector and sets default GameMode to use CVT"),
		FSlateIcon(),
		FUIAction(FExecuteAction::CreateRaw(this, &FCavrnusCVTEditorModule::SetupLevel))
	);

	MenuBuilder.AddMenuEntry(
	LOCTEXT("SetupLevel", "Convert ALL StaticMeshActors to movable"),
	LOCTEXT("SetupLevelTooltip", "Setup project settings"),
	FSlateIcon(),
	FUIAction(FExecuteAction::CreateRaw(this, &FCavrnusCVTEditorModule::ConvertStaticMeshActors))
	);
}

void FCavrnusCVTEditorModule::AddCollisionProfiles()
{
	RegisterCollisionChannelIfMissing("Annotation");
	RegisterCollisionChannelIfMissing("AnnotationStroke");
	RegisterCollisionChannelIfMissing("SnapShot");
	RegisterCollisionChannelIfMissing("DimenssionTool");
	RegisterCollisionChannelIfMissing("Gizmo");
	RegisterCollisionChannelIfMissing("Xrayed");
	RegisterCollisionChannelIfMissing("UI_A");
}

void FCavrnusCVTEditorModule::RegisterCollisionChannelIfMissing(const FString& ChannelName)
{
	const FString ConfigPath = FPaths::ProjectConfigDir() / TEXT("DefaultEngine.ini");
	const FString SectionHeader = TEXT("[/Script/Engine.CollisionProfile]");

	FString FileContents;
	if (!FFileHelper::LoadFileToString(FileContents, *ConfigPath))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to read DefaultEngine.ini"));
		return;
	}

	// Bail if the name already exists
	if (FileContents.Contains(FString::Printf(TEXT("Name=\"%s\""), *ChannelName)))
	{
		UE_LOG(LogTemp, Log, TEXT("Collision channel '%s' already exists."), *ChannelName);
		return;
	}

	// Find a free ECC_GameTraceChannel1–18
	int32 FreeChannelIndex = -1;
	for (int32 i = 1; i <= 18; ++i)
	{
		FString ChannelEnum = FString::Printf(TEXT("ECC_GameTraceChannel%d"), i);
		if (!FileContents.Contains(ChannelEnum))
		{
			FreeChannelIndex = i;
			break;
		}
	}

	if (FreeChannelIndex == -1)
	{
		UE_LOG(LogTemp, Error, TEXT("No free ECC_GameTraceChannel slots available."));
		return;
	}

	const FString ChannelEnumString = FString::Printf(TEXT("ECC_GameTraceChannel%d"), FreeChannelIndex);
	const FString NewLine = FString::Printf(
		TEXT("+DefaultChannelResponses=(Channel=%s,DefaultResponse=ECR_Block,bTraceType=False,bStaticObject=False,Name=\"%s\")"),
		*ChannelEnumString,
		*ChannelName
	);

	// Split into lines for processing
	TArray<FString> Lines;
	FileContents.ParseIntoArrayLines(Lines);

	int32 InsertIndex = -1;

	// Try to find the insertion point (first line after the section header)
	for (int32 i = 0; i < Lines.Num(); ++i)
	{
		if (Lines[i].TrimStartAndEnd().Equals(SectionHeader))
		{
			InsertIndex = i + 1;
			break;
		}
	}

	// Inject line or create the section if it doesn’t exist
	if (InsertIndex != -1)
	{
		Lines.Insert(NewLine, InsertIndex);
	}
	else
	{
		Lines.Add(TEXT(""));
		Lines.Add(SectionHeader);
		Lines.Add(NewLine);
	}

	// Reassemble and save
	FString NormalizedContents = FString::Join(Lines, TEXT("\n")); // force LF
	FFileHelper::SaveStringToFile(NormalizedContents, *ConfigPath, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), FILEWRITE_None);
}

bool FCavrnusCVTEditorModule::DoesProfileExistInConfig(FName ProfileName)
{
	const FString ConfigPath = FPaths::ProjectConfigDir() / TEXT("DefaultEngine.ini");

	TArray<FString> SectionLines;
	if (GConfig->GetSection(TEXT("/Script/Engine.CollisionProfile"), SectionLines, ConfigPath))
	{
		const FString Target = FString::Printf(TEXT("Profiles=(Name=%s"), *ProfileName.ToString());
		for (const FString& Line : SectionLines)
		{
			if (Line.StartsWith(Target))
			{
				return true;
			}
		}
	}
	return false;
}

void FCavrnusCVTEditorModule::ConvertStaticMeshActors()
{
	if (GEditor)
	{
		if (UWorld* World = GEditor->GetEditorWorldContext().World())
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.OverrideLevel = World->PersistentLevel;
			for (TActorIterator<AStaticMeshActor> It(World); It; ++It)
			{
				if (AStaticMeshActor* StaticMeshActor = *It)
				{
					// Get the StaticMeshComponent
					UStaticMeshComponent* StaticMeshComponent = StaticMeshActor->GetStaticMeshComponent();

					if (StaticMeshComponent && StaticMeshComponent->Mobility == EComponentMobility::Static)
					{
						// Set the mobility to Movable
						StaticMeshComponent->SetMobility(EComponentMobility::Movable);

						// Mark the actor as modified in the editor so the change is saved
						StaticMeshActor->MarkPackageDirty();
					}
				}
			}
		}
	}
}

void FCavrnusCVTEditorModule::TryAddCvtActorManager()
{
	if (GEditor)
	{
		if (UWorld* World = GEditor->GetEditorWorldContext().World())
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.OverrideLevel = World->PersistentLevel;

			bool bWorldNeedsManager = true;
			for (TActorIterator<AActor> It(World, ACavrnusCVTManager::StaticClass()); It; ++It)
			{
				bWorldNeedsManager = false;
			}

			if (bWorldNeedsManager)
			{
				if (UClass* Found = GetDefaultBlueprint(TEXT("/CavrnusCVT/CavrnusIntegration/Commands/A_CavrnusCVTManager.A_CavrnusCVTManager_C"), AActor::StaticClass()))
				{
					World->SpawnActor(Found);
				}
			}
		}
	}
}

void FCavrnusCVTEditorModule::SetupLevel()
{
	SetConnectorSettings();
	TryAddCvtActorManager();
	SetGameMode();
}

void FCavrnusCVTEditorModule::SetConnectorSettings()
{
	if (auto* Settings = GetMutableDefault<UCavrnusConnectorSettings>())
	{
		Settings->MemberLoginMenu = GetDefaultBlueprint(TEXT("/CavrnusConnector/UI/Menus/LoginMenus/WBP_MemberLogin.WBP_MemberLogin_C"), UUserWidget::StaticClass());
		Settings->ServerSelectionMenu = GetDefaultBlueprint(TEXT("/CavrnusConnector/UI/Menus/ServerMenu/WBP_ServerSelectionMenu.WBP_ServerSelectionMenu_C"), UCavrnusServerSelectionWidget::StaticClass());
		Settings->GuestJoinMenu = GetDefaultBlueprint(TEXT("/CavrnusConnector/UI/Menus/LoginMenus/WBP_GuestLogin.WBP_GuestLogin_C"), UCavrnusGuestLoginWidget::StaticClass());
		Settings->SpacesListMenu = GetDefaultBlueprint(TEXT("/CavrnusConnector/UI/Menus/SpaceListMenu/WBP_SpaceSelection.WBP_SpaceSelection_C"), UUserWidget::StaticClass());
		Settings->LoadingWidgetMenu = GetDefaultBlueprint(TEXT("/CavrnusConnector/UI/Menus/LoadingMenu/WBP_LoadingWidget.WBP_LoadingWidget_C"), UUserWidget::StaticClass());
		Settings->AuthenticationWidgetMenu = GetDefaultBlueprint(TEXT("/CavrnusConnector/UI/Menus/LoadingMenu/WBP_AuthenticationWidget.WBP_AuthenticationWidget_C"), UUserWidget::StaticClass());

		auto Ui = GetDefaultBlueprint(TEXT("/CavrnusConnector/UI/MinimalUI/WBP_MinimalUI.WBP_MinimalUI_C"), UUserWidget::StaticClass());
		if (Settings->WidgetsToLoad.IsEmpty())
		{
			Settings->WidgetsToLoad.Add(Ui);
		} else
		{
			for (auto W : Settings->WidgetsToLoad)
			{
				if (W)
				{
					if (!W.Get()->GetClassPathName().ToString().Contains("Minimal"))
						Settings->WidgetsToLoad.Add(Ui);
				}
			}
		}
	}
}

void FCavrnusCVTEditorModule::SetGameMode()
{
	const UWorld* World = GEditor->GetEditorWorldContext().World();
	
	if (!World)
	{
		UE_LOG(LogCavrnusCVTEditor, Error, TEXT("World is null"));
		return;
	}

	// Get the current level
	if (const ULevel* CurrentLevel = World->GetCurrentLevel(); !CurrentLevel)
	{
		UE_LOG(LogCavrnusCVTEditor, Error, TEXT("Current level is null"));
		return;
	}

	UClass* FoundGameMode = GetDefaultBlueprint(TEXT("/CavrnusCVT/CollaborativeViewer/Blueprints/GameMode/BP_CollaborativeViewer_GameMode.BP_CollaborativeViewer_GameMode_C"), AGameModeBase::StaticClass());
	if (FoundGameMode && FoundGameMode->IsChildOf(AGameModeBase::StaticClass()))
	{
		if (AWorldSettings* WorldSettings = World->GetWorldSettings())
		{
			WorldSettings->DefaultGameMode = FoundGameMode;
		}
		else
		{
			UE_LOG(LogCavrnusCVTEditor, Error, TEXT("World settings not found"));
		}
	}
	else
	{
		UE_LOG(LogCavrnusCVTEditor, Warning, TEXT("FoundGameMode is NOT a subclass of AGameModeBase"));
	}
}

UClass* FCavrnusCVTEditorModule::GetDefaultBlueprint(const FString& Path, UClass* BaseClass)
{
	// Use BP as default value
	UClass* LoadedBlueprintClass = StaticLoadClass(BaseClass, nullptr, *Path, nullptr, LOAD_None, nullptr);
	if (!LoadedBlueprintClass)
	{
		UE_LOG(LogCavrnusCVTEditor, Error, TEXT("Blueprint asset failed to load from path: %s, base class name: %s"), *Path, *BaseClass->GetName());
	}

	return LoadedBlueprintClass;
}