// Copyright(c) Cavrnus. All rights reserved.
// Includes
#include "CavrnusConnectorEditorModule.h"
#include "Interfaces/IMainFrameModule.h"
#include "Editor/EditorEngine.h"

#include <Misc/Paths.h>
#include <LevelEditor.h>
#include <EngineUtils.h> // for TActorIterator<>
#include <ToolMenus.h>
#include <ContentBrowserMenuContexts.h>
#include "CavrnusSpatialConnector.h"
#include "CavrnusSplashScreenWidget.h"
#include <EditorUtilitySubsystem.h>
#include <EditorUtilityWidget.h>
#include <EditorUtilityWidgetBlueprint.h>
#include <Framework/Docking/TabManager.h>
#include <Widgets/Docking/SDockTab.h>
#include "ValueSyncs/CavrnusValueSyncBase.h"
#include "CavrnusPropertiesContainer.h"
#include <Engine/Blueprint.h>
#include <Engine/SimpleConstructionScript.h>
#include <Engine/SCS_Node.h>
#include <Kismet2/BlueprintEditorUtils.h>
#include <Tools/Modes.h>

#define LOCTEXT_NAMESPACE "CavrnusConnectorEditor"

IMPLEMENT_MODULE(FCavrnusConnectorEditorModule, CavrnusConnectorEditor)
DEFINE_LOG_CATEGORY(LogCavrnusConnectorEditor);
static const FName LevelEditorModuleName("LevelEditor");
//===============================================================
FCavrnusConnectorEditorModule::FCavrnusConnectorEditorModule()
{

}

//===============================================================
FCavrnusConnectorEditorModule::~FCavrnusConnectorEditorModule()
{
}


//===============================================================
void FCavrnusConnectorEditorModule::StartupModule()
{
	RegisterStartupTab();
	HookAutoAddContainerComponents();

	FLevelEditorModule& LevelEditorModule =
		FModuleManager::LoadModuleChecked<FLevelEditorModule>
		(LevelEditorModuleName);

	TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());

	MenuExtender->AddMenuBarExtension(
		"Help",
		EExtensionHook::After,
		nullptr,
		FMenuBarExtensionDelegate::CreateRaw(this, &FCavrnusConnectorEditorModule::AddTopRibbonMenu)
	);

	LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);
	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FCavrnusConnectorEditorModule::RegisterToolsMenus));
}

//===============================================================
void FCavrnusConnectorEditorModule::ShutdownModule()
{
	UnregisterStartupTab();

	UnhookAutoAddContainerComponents();
}

void FCavrnusConnectorEditorModule::HookAutoAddContainerComponents()
{
	PostEngineInitDelegateHandle = FCoreDelegates::OnPostEngineInit.AddLambda([this]() {
		if (GEditor)
		{
			if (UAssetEditorSubsystem* EditorSubSystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>())
			{
				EditorSubSystem->OnAssetOpenedInEditor().AddRaw(this, &FCavrnusConnectorEditorModule::OnAssetOpened);
				EditorSubSystem->OnAssetEditorRequestClose().AddRaw(this, &FCavrnusConnectorEditorModule::OnAssetRequestClose);
			}
		}
		});
}

void FCavrnusConnectorEditorModule::UnhookAutoAddContainerComponents()
{
	if (PostEngineInitDelegateHandle.IsValid())
	{
		FCoreDelegates::OnPostEngineInit.Remove(PostEngineInitDelegateHandle);
	}

	if (GEditor)
	{
		GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OnAssetOpenedInEditor().RemoveAll(this);
	}
}

void FCavrnusConnectorEditorModule::OnAssetOpened(UObject* Asset, IAssetEditorInstance* EditorInstance)
{
	// Need to get hold of the blueprint and register to OnBlueprintChanged();
	if (UBlueprint* Blueprint = Cast<UBlueprint>(Asset))
	{
		Blueprint->OnChanged().AddRaw(this, &FCavrnusConnectorEditorModule::OnBlueprintChanged);
	}
}

void FCavrnusConnectorEditorModule::OnAssetRequestClose(UObject* Asset, EAssetEditorCloseReason CloseReason)
{
	if (UBlueprint* Blueprint = Cast<UBlueprint>(Asset))
	{
		Blueprint->OnChanged().RemoveAll(this);
	}
}

void FCavrnusConnectorEditorModule::OnBlueprintChanged(UBlueprint* Blueprint)
{
	// Search the components and auto attach containers
	if (!Blueprint || !Blueprint->SimpleConstructionScript)
		return;

	bool bStructurallyModified = false;
	TArray<USCS_Node*> BlueprintNodes = Blueprint->SimpleConstructionScript->GetAllNodes();
	TArray<USCS_Node*> NodesNeedingContainer;
	for (USCS_Node* Node : BlueprintNodes)
	{
		if (!Node->ComponentTemplate->IsA<UCavrnusValueSyncBase>())
		{
			continue;
		}

		// Need to check siblings for a pre-existing container
		bool bParentNeedsContainer = true;
		AActor* CDO = Cast<AActor>(Blueprint->GeneratedClass->GetDefaultObject(false));
		if (CDO)
		{
			TArray<USCS_Node*> SiblingNodes;
			if (Node->bIsParentComponentNative)
			{
				USceneComponent* ParentContentTemplate = Node->GetParentComponentTemplate(Blueprint);
				if (ParentContentTemplate)
				{
					SiblingNodes = BlueprintNodes.FilterByPredicate([Node, Blueprint, ParentContentTemplate](USCS_Node* BPNode) {
						return BPNode != Node && BPNode->GetParentComponentTemplate(Blueprint) == ParentContentTemplate;
						});
				}
			}
			else
			{
				USCS_Node* Parent = Blueprint->SimpleConstructionScript->FindParentNode(Node);
				if (Parent)
				{
					SiblingNodes = Parent->ChildNodes;
				}
			}

			for (USCS_Node* Sibling : SiblingNodes)
			{
				if (Sibling->ComponentTemplate->IsA<UCavrnusPropertiesContainer>())
				{
					bParentNeedsContainer = false;
					break;
				}
			}
		}

		if (bParentNeedsContainer)
		{
			UCavrnusPropertiesContainer* PropertiesContainer = NewObject<UCavrnusPropertiesContainer>();

			USCS_Node* NewNode = Blueprint->SimpleConstructionScript->CreateNode(PropertiesContainer->GetClass(), *PropertiesContainer->GetName());
			if (Node->bIsParentComponentNative)
			{
				Blueprint->SimpleConstructionScript->AddNode(NewNode);
				USceneComponent* ParentComponentTemplate = Node->GetParentComponentTemplate(Blueprint);
				if (ParentComponentTemplate)
				{
					PropertiesContainer->ContainerName = GenerateContainerName(ParentComponentTemplate, Blueprint);
					NewNode->SetParent(Cast<USceneComponent>(ParentComponentTemplate));
				}
			}
			else
			{
				USCS_Node* Parent = Blueprint->SimpleConstructionScript->FindParentNode(Node);
				PropertiesContainer->ContainerName = GenerateContainerName(Parent, Blueprint);
				if (Parent)
				{
					Parent->AddChildNode(NewNode);
				}
			}

			UEditorEngine::CopyPropertiesForUnrelatedObjects(PropertiesContainer, NewNode->ComponentTemplate);

			bStructurallyModified = true;

		}
	}

	if (bStructurallyModified)
	{
		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);
	}
}

FString FCavrnusConnectorEditorModule::GenerateContainerName(USceneComponent* ParentContentTemplate, UBlueprint* Blueprint) const
{
	USceneComponent* Current = ParentContentTemplate;
	FString ContainerName;

	do
	{
		FString ObjectName = Current->GetName();
		ObjectName.RemoveFromEnd(TEXT("_GEN_VARIABLE"));
		if (ContainerName.IsEmpty())
		{
			ContainerName = ObjectName;
		}
		else
		{
			ContainerName = ObjectName + "/" + ContainerName;
		}

		Current = Current->GetAttachParent();
	} while (Current != nullptr);

	ContainerName = Blueprint->GetName() + "/" + ContainerName;

	return ContainerName;
}

FString FCavrnusConnectorEditorModule::GenerateContainerName(USCS_Node* ContainerParentNode, UBlueprint* Blueprint) const
{
	USCS_Node* CurrentNode = ContainerParentNode;
	FString ContainerName;

	do
	{
		FString ObjectName = CurrentNode->ComponentTemplate->GetName();
		ObjectName.RemoveFromEnd(TEXT("_GEN_VARIABLE"));
		if (ContainerName.IsEmpty())
		{
			ContainerName = ObjectName;
		}
		else
		{
			ContainerName = ObjectName + "/" + ContainerName;
		}

		CurrentNode = Blueprint->SimpleConstructionScript->FindParentNode(CurrentNode);
	} while (CurrentNode != nullptr);

	ContainerName = Blueprint->GetName() + "/" + ContainerName;

	return ContainerName;
}

void FCavrnusConnectorEditorModule::RegisterStartupTab()
{
	FLevelEditorModule& LevelEditorModule =
		FModuleManager::LoadModuleChecked<FLevelEditorModule>
		(LevelEditorModuleName);

	BeginPIEDelegateHandle = FEditorDelegates::BeginPIE.AddLambda([this](const bool bIsSimulating) {
		CloseSplashWindow();
		});

	EnginePreExitDelegateHandle = FCoreDelegates::OnEnginePreExit.AddLambda([this]() {
		CloseSplashWindow();
		});

	IMainFrameModule& MainFrameModule = FModuleManager::LoadModuleChecked<IMainFrameModule>(TEXT("MainFrame"));
	MainFrameCreatedDelegateHandle = MainFrameModule.OnMainFrameCreationFinished().AddLambda([this](TSharedPtr<SWindow> Window, bool bIsNewProjectWindow) {
		if (!bIsNewProjectWindow && GEditor)
		{
			// Use SetTimerForNextTick to let other places handle events in response to the main frame 
			// creation finishing
			// e.g. the plugin module shows notifications when the main frame creation is done
			// which can steal focus away from the parent window of the splash screen tab.
			TSharedRef<FTimerManager> WorldTimerManager = GEditor->GetTimerManager();
			WorldTimerManager->SetTimerForNextTick([this]() {
				DrawAttentionToSplashScreen();
				});
		}
		});

	static bool bShowSplashScreen = ShouldShowSplashScreen();
	SetShowSplashScreen(bShowSplashScreen);
	bShowSplashScreen = false;

	ShowSplashScreenDelegateHandle = LevelEditorModule.OnTabManagerChanged().AddLambda([this]
	{
		if (bShowSplashScreen)
		{
			SetShowSplashScreen(true);
			bShowSplashScreen = false;
		}
	});
}

void FCavrnusConnectorEditorModule::UnregisterStartupTab()
{
	FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>(LevelEditorModuleName);
	if (ShowSplashScreenDelegateHandle.IsValid())
	{
		LevelEditorModule.OnTabManagerChanged().Remove(ShowSplashScreenDelegateHandle);
	}

	IMainFrameModule& MainFrameModule = FModuleManager::LoadModuleChecked<IMainFrameModule>(TEXT("MainFrame"));
	if (MainFrameCreatedDelegateHandle.IsValid())
	{
		MainFrameModule.OnMainFrameCreationFinished().Remove(MainFrameCreatedDelegateHandle);
	}

	if (BeginPIEDelegateHandle.IsValid())
	{
		FEditorDelegates::BeginPIE.Remove(BeginPIEDelegateHandle);
	}

	if (EnginePreExitDelegateHandle.IsValid())
	{
		FCoreDelegates::OnEnginePreExit.Remove(EnginePreExitDelegateHandle);
	}
}

void FCavrnusConnectorEditorModule::DrawAttentionToSplashScreen()
{
	// Bring tab to front
	FLevelEditorModule& LevelEditorModule =
		FModuleManager::LoadModuleChecked<FLevelEditorModule>
		(LevelEditorModuleName);
	TSharedPtr<FTabManager> LevelEditorTabManager = LevelEditorModule.GetLevelEditorTabManager();
	TSharedPtr<SDockTab> FoundTab = LevelEditorTabManager->FindExistingLiveTab(SplashScreenTabID);
	if (FoundTab)
	{
		FoundTab->SetLabel(FText::FromString(TEXT("Cavrnus Spatial Connector")));
		if (TSharedPtr<SWindow> Parent = FoundTab->GetParentWindow())
		{
			Parent->GetNativeWindow()->SetWindowFocus();
			FoundTab->DrawAttention();
		}
		else
		{
			CloseSplashWindow();
		}
	}
}

void FCavrnusConnectorEditorModule::CloseSplashWindow()
{
	if (GEditor)
	{
		if (UEditorUtilitySubsystem* EditorUtilitySubsystem = GEditor->GetEditorSubsystem<UEditorUtilitySubsystem>())
		{
			EditorUtilitySubsystem->CloseTabByID(SplashScreenTabID);
		}
	}

	SplashScreenTabID = FName();

	FLevelEditorModule& LevelEditorModule =
		FModuleManager::LoadModuleChecked<FLevelEditorModule>
		(LevelEditorModuleName);
	TSharedPtr<FTabManager> LevelEditorTabManager = LevelEditorModule.GetLevelEditorTabManager();
	if (LevelEditorTabManager)
	{
		LevelEditorTabManager->SavePersistentLayout();
	}
}

bool FCavrnusConnectorEditorModule::ShouldShowSplashScreen() const
{
	bool bShowSplashScreenOnStartup = true;
	FString OutValue;
	if (FPlatformMisc::GetStoredValue(TEXT("Cavrnus"), TEXT("UE"), TEXT("ShowSplashScreenOnStartup"), OutValue))
	{
		bShowSplashScreenOnStartup = OutValue.ToBool();
	}

	return bShowSplashScreenOnStartup;
}

void FCavrnusConnectorEditorModule::SetShowSplashScreen(bool bShowOnStartup)
{
	FString Key = TEXT("ShowSplashScreenOnStartup");
	FString InValue = bShowOnStartup ? TEXT("True") : TEXT("False");
	FPlatformMisc::SetStoredValue(TEXT("Cavrnus"), TEXT("UE"), TEXT("ShowSplashScreenOnStartup"), InValue);
}

void FCavrnusConnectorEditorModule::AddTopRibbonMenu(FMenuBarBuilder& MenuBuilder)
{
	MenuBuilder.AddPullDownMenu(
		LOCTEXT("MenuLocKey", "Cavrnus"),
		LOCTEXT("MenuTooltipKey", "Opens menu for CavrnusConnector plugin"),
		FNewMenuDelegate::CreateRaw(this, &FCavrnusConnectorEditorModule::AddCavrnusToolbarEntry),
		FName(TEXT("Cavrnus")),
		FName(TEXT("Cavrnus Help Menu")));
}

void FCavrnusConnectorEditorModule::RegisterToolsMenus()
{
	UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("ContentBrowser.AssetContextMenu");
	FToolMenuSection& Section = Menu->FindOrAddSection("AssetContextCavrnusActions");
	Section.Label = FText::FromString("Cavrnus");
	FToolMenuEntry& Entry = Section.AddDynamicEntry(
		"AssetManagerEditorCavrnusCommands",
		FNewToolMenuSectionDelegate::CreateRaw(this, &FCavrnusConnectorEditorModule::AddCavrnusToolsOptions));
}

void FCavrnusConnectorEditorModule::AddCavrnusToolsOptions(FToolMenuSection& InSection)
{
	UContentBrowserAssetContextMenuContext* Context = InSection.FindContext<UContentBrowserAssetContextMenuContext>();
	if (Context)
	{
		FToolUIActionChoice AddCavrnusSpawnable(FExecuteAction::CreateLambda([this, Context]()
			{
				TArray<UObject*> SelectedObjects = Context->GetSelectedObjects();
				for (UObject* SelectedObject : SelectedObjects)
				{
					FAssetData AssetData(SelectedObject);

					// Generate unique key
					int IdentifierIndex = 0;
					FString AvailableIdentifierName = SelectedObject->GetName();
					while (SpawnableIdentifiers.Contains(AvailableIdentifierName))
					{
						AvailableIdentifierName = SelectedObject->GetName() + FString::FromInt(IdentifierIndex++);
					}

					// If identifier != the asset name then confirm if want to continue with
					// the generated name
					if (AvailableIdentifierName != SelectedObject->GetName())
					{
						FString ConfirmationString = FString::Printf(TEXT("This asset is already spawnable with unique identifier: %s. Do you want to add this asset again with generated identifier: %s?"), *SelectedObject->GetName(), *AvailableIdentifierName);
						EAppReturnType::Type Result = FMessageDialog::Open(EAppMsgType::OkCancel, FText::FromString(ConfirmationString));
						if (Result != EAppReturnType::Ok)
						{
							continue;
						}
					}

					FSoftObjectPath ObjectPath = AssetData.ToSoftObjectPath();
					UBlueprintCore* LoadedObject = Cast<UBlueprintCore>(ObjectPath.TryLoad());
					checkf(LoadedObject->GeneratedClass->IsChildOf<AActor>(), TEXT("Can only spawn actor types"));
					TSubclassOf<AActor> BlueprintClass(LoadedObject->GeneratedClass);

					SpawnableIdentifiers.Add(AvailableIdentifierName, BlueprintClass);
				}

				// Find instance of CavrnusSpatialConnector and update
				if (UWorld* World = GEditor->GetEditorWorldContext().World())
				{
					for (TActorIterator<ACavrnusSpatialConnector> ActorItr((World)); ActorItr; ++ActorItr)
					{
						ActorItr->SpawnableIdentifiers = SpawnableIdentifiers;
					}
				}
			}));
		InSection.AddEntry(FToolMenuEntry::InitMenuEntry(FName("Make Cavrnus Spawnable"), FText::FromString("Make Cavrnus Spawnable"), FText::FromString("Add Cavrnus identifier"), FSlateIcon(), AddCavrnusSpawnable));
	}
}

void FCavrnusConnectorEditorModule::AddCavrnusToolbarEntry(FMenuBuilder& MenuBuilder)
{
	MenuBuilder.AddMenuEntry(
		FText::FromString(TEXT("Add CavrnusSpatialConnector to current level")),
		FText::GetEmpty(),
		FSlateIcon(),
		FUIAction(FExecuteAction::CreateLambda([this]
		{
			AddCavrnusSpatialConnectorToLevel();
		})),
		NAME_None,
		EUserInterfaceActionType::Button
	);

	MenuBuilder.AddMenuEntry(
		FText::FromString(TEXT("Welcome")),
		FText::GetEmpty(),
		FSlateIcon(),
		FUIAction(FExecuteAction::CreateLambda([this]
		{
			SetWelcomeScreenState(true);
		})),
		NAME_None,
		EUserInterfaceActionType::Button
	);
}

void FCavrnusConnectorEditorModule::SetWelcomeScreenState(bool bToShow)
{
	if (bToShow)
	{
		SplashScreenTabID = TEXT("SplashScreenTab");
		UEditorUtilitySubsystem* EditorUtilitySubsystem = GEditor->GetEditorSubsystem<UEditorUtilitySubsystem>();
		const FString BlueprintPath = TEXT("/CavrnusConnector/UI/EditorMenus/EUW_SplashScreen");
		
		UEditorUtilityWidgetBlueprint* LoadedBlueprintClass = LoadObject<UEditorUtilityWidgetBlueprint>(NULL, *BlueprintPath, NULL, LOAD_None, NULL);
		if (LoadedBlueprintClass)
		{
			UEditorUtilityWidget* SpawnedWidget = EditorUtilitySubsystem->SpawnAndRegisterTabAndGetID(LoadedBlueprintClass, SplashScreenTabID);
			if (UCavrnusSplashScreenWidget* SplashScreen = Cast<UCavrnusSplashScreenWidget>(SpawnedWidget))
			{
				SplashScreen->TabID = SplashScreenTabID;
				DrawAttentionToSplashScreen();
			}
		}
	}	
}

void FCavrnusConnectorEditorModule::AddCavrnusSpatialConnectorToLevel()
{
	if (GEditor)
	{
		if (UWorld* World = GEditor->GetEditorWorldContext().World())
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.OverrideLevel = World->PersistentLevel;

			bool bWorldNeedsSpatialConnector = true;
			for (TActorIterator<AActor> It(World, ACavrnusSpatialConnector::StaticClass()); It; ++It)
			{
				bWorldNeedsSpatialConnector = false;
			}

			if (bWorldNeedsSpatialConnector)
			{
				if (ACavrnusSpatialConnector* CavrnusSpatialConnector = World->SpawnActor<ACavrnusSpatialConnector>(SpawnParams))
				{
					CavrnusSpatialConnector->SetIsSpatiallyLoaded(false);
					CavrnusSpatialConnector->SpawnableIdentifiers = SpawnableIdentifiers;
					CavrnusSpatialConnector->GuestName = "Unreal Guest";
					CavrnusSpatialConnector->MemberLoginMenu = GetDefaultBlueprint(TEXT("/CavrnusConnector/UI/Menus/LoginMenus/WBP_MemberLogin.WBP_MemberLogin_C"), UCavrnusLoginWidget::StaticClass());
					CavrnusSpatialConnector->GuestJoinMenu = GetDefaultBlueprint(TEXT("/CavrnusConnector/UI/Menus/LoginMenus/WBP_GuestLogin.WBP_GuestLogin_C"), UCavrnusGuestLoginWidget::StaticClass());
					CavrnusSpatialConnector->SpaceJoinMenu = GetDefaultBlueprint(TEXT("/CavrnusConnector/UI/Menus/SpaceListMenu/WBP_SpaceSelection.WBP_SpaceSelection_C"), UCavrnusSpaceListWidget::StaticClass());
					CavrnusSpatialConnector->LoadingWidgetClass = GetDefaultBlueprint(TEXT("/CavrnusConnector/UI/Menus/LoadingMenu/WBP_LoadingWidget.WBP_LoadingWidget_C"), UUserWidget::StaticClass());
					CavrnusSpatialConnector->AuthenticationWidgetClass = GetDefaultBlueprint(TEXT("/CavrnusConnector/UI/Menus/LoadingMenu/WBP_AuthenticationWidget.WBP_AuthenticationWidget_C"), UUserWidget::StaticClass());
					CavrnusSpatialConnector->RemoteAvatarClass = GetDefaultBlueprint(TEXT("/CavrnusConnector/Pawns/Blueprints/BP_Cavrnus_RemoteAvatarLoader.BP_Cavrnus_RemoteAvatarLoader_C"), AActor::StaticClass());
					
					TArray<TSubclassOf<UUserWidget>> WidgetsToLoad;
					WidgetsToLoad.Add(GetDefaultBlueprint(TEXT("/CavrnusConnector/UI/MinimalUI/WBP_MinimalUI.WBP_MinimalUI_C"), UUserWidget::StaticClass()));
					CavrnusSpatialConnector->WidgetsToLoad = WidgetsToLoad;

					FString SavedServer;
					FPlatformMisc::GetStoredValue(TEXT("Cavrnus"), TEXT("UE"), TEXT("SavedServerName"), SavedServer);
					CavrnusSpatialConnector->MyServer = SavedServer;
				}
			}
			else
			{
				UE_LOG(LogCavrnusConnectorEditor, Warning, TEXT("World already contains a CavrnusSpatialConnector"));
			}

		}
	}
}

UClass* FCavrnusConnectorEditorModule::GetDefaultBlueprint(const FString& Path, UClass* BaseClass) const
{
	// Use BP as default value
	UClass* LoadedBlueprintClass = StaticLoadClass(BaseClass, nullptr, *Path, nullptr, LOAD_None, nullptr);
	if (!LoadedBlueprintClass)
	{
		UE_LOG(LogCavrnusConnectorEditor, Error, TEXT("Blueprint asset failed to load from path: %s, base class name: %s"), *Path, *BaseClass->GetName());
	}

	return LoadedBlueprintClass;
}

FString FCavrnusConnectorEditorModule::GetPluginPath()
{
	static FString PluginPath;

	if (PluginPath.IsEmpty())
	{
		FString ProjectPath = FPaths::ProjectPluginsDir() / TEXT("CavrnusConnector");
		FString EnginePath = FPaths::EnginePluginsDir() / TEXT("Marketplace") / TEXT("CavrnusConnector");

		if (IFileManager::Get().DirectoryExists(*ProjectPath))
		{
			PluginPath = ProjectPath;
		}
		else
		{
			PluginPath = EnginePath;
		}
	}

	return PluginPath;
}

#undef LOCTEXT_NAMESPACE