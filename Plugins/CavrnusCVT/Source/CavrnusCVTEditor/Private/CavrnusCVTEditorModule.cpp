#include "CavrnusCVTEditorModule.h"

#include "EngineUtils.h"
#include "LevelEditor.h"
#include "CavrnusCVT/CavrnusCVTManager.h"
#include "Engine/StaticMeshActor.h"
#include "GameFramework/GameModeBase.h"
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
		LOCTEXT("SetupLevel", "Setup level for Cavrnus Collab Viewer"),
		LOCTEXT("SetupLevelTooltip", "Configures SpatialConnector and sets default GameMode to use CVT"),
		FSlateIcon(),
		FUIAction(FExecuteAction::CreateRaw(this, &FCavrnusCVTEditorModule::SetupLevel))
	);

	MenuBuilder.AddMenuEntry(
	LOCTEXT("SetupLevel", "Convert ALL StaticMeshActors to movable"),
	LOCTEXT("SetupLevelTooltip", "Allows for objects in level to be manipulated via CVT move command"),
	FSlateIcon(),
	FUIAction(FExecuteAction::CreateRaw(this, &FCavrnusCVTEditorModule::ConvertStaticMeshActors))
);
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

void FCavrnusCVTEditorModule::TryAddManager()
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

void FCavrnusCVTEditorModule::TryAddSpatialConnector()
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
					CavrnusSpatialConnector->GuestName = "CVT Guest";
					CavrnusSpatialConnector->MemberLoginMenu = GetDefaultBlueprint(TEXT("/CavrnusConnector/UI/Menus/LoginMenus/WBP_MemberLogin.WBP_MemberLogin_C"), UUserWidget::StaticClass());
					CavrnusSpatialConnector->ServerSelectionMenu = GetDefaultBlueprint(TEXT("/CavrnusConnector/UI/Menus/ServerMenu/WBP_ServerSelectionMenu.WBP_ServerSelectionMenu_C"), UCavrnusServerSelectionWidget::StaticClass());
					CavrnusSpatialConnector->GuestJoinMenu = GetDefaultBlueprint(TEXT("/CavrnusConnector/UI/Menus/LoginMenus/WBP_GuestLogin.WBP_GuestLogin_C"), UCavrnusGuestLoginWidget::StaticClass());
					CavrnusSpatialConnector->SpaceJoinMenu = GetDefaultBlueprint(TEXT("/CavrnusConnector/UI/Menus/SpaceListMenu/WBP_SpaceSelection.WBP_SpaceSelection_C"), UUserWidget::StaticClass());
					CavrnusSpatialConnector->LoadingWidgetClass = GetDefaultBlueprint(TEXT("/CavrnusConnector/UI/Menus/LoadingMenu/WBP_LoadingWidget.WBP_LoadingWidget_C"), UUserWidget::StaticClass());
					CavrnusSpatialConnector->AuthenticationWidgetClass = GetDefaultBlueprint(TEXT("/CavrnusConnector/UI/Menus/LoadingMenu/WBP_AuthenticationWidget.WBP_AuthenticationWidget_C"), UUserWidget::StaticClass());
					CavrnusSpatialConnector->RemoteAvatarClass = GetDefaultBlueprint(TEXT("/CavrnusCVT/CavrnusIntegration/Pawns/BP_CVT_RemoteAvatarLoader.BP_CVT_RemoteAvatarLoader_C"), AActor::StaticClass());
					
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
				UE_LOG(LogCavrnusCVTEditor, Warning, TEXT("World already contains a CavrnusSpatialConnector"));
			}
		}
	}
}

void FCavrnusCVTEditorModule::SetupLevel()
{
	TryAddSpatialConnector();
	TryAddManager();
	SetGameMode();
	
	UClass* SpatialConnector = ACavrnusSpatialConnector::StaticClass();
	if (const UWorld* World = GEditor->GetEditorWorldContext().World())
	{
		for (TActorIterator<AActor> Target(World, SpatialConnector); Target; ++Target)
		{
			if (AActor* Actor = *Target)
			{
				// Log the name of the found actor
				UE_LOG(LogCavrnusCVTEditor, Warning, TEXT("Found actor: %s"), *Actor->GetName());
                
				// If you need to cast to your specific type
				if (ACavrnusSpatialConnector* SC = Cast<ACavrnusSpatialConnector>(Actor))
				{
					// Perform operations on the casted actor
					UE_LOG(LogCavrnusCVTEditor, Warning, TEXT("Successfully casted actor: %s"), *Actor->GetName());

					SC->SpawnableIdentifiers.Empty();
					AddSpawnableItem(*SC, "BP_Cavrnus_DimensionLoader", TEXT("/CavrnusCVT/CavrnusIntegration/Commands/Dimension/BP_Cavrnus_DimensionLoader.BP_Cavrnus_DimensionLoader_C"));
					AddSpawnableItem(*SC, "BP_Cavrnus_AnnotationLoaderText", TEXT("/CavrnusCVT/CavrnusIntegration/Commands/Annotations/BP_Cavrnus_AnnotationLoaderText.BP_Cavrnus_AnnotationLoaderText_C"));
					AddSpawnableItem(*SC, "BP_Cavrnus_AnnotationLoaderStroke", TEXT("/CavrnusCVT/CavrnusIntegration/Commands/Annotations/BP_Cavrnus_AnnotationLoaderStroke.BP_Cavrnus_AnnotationLoaderStroke_C"));
					AddSpawnableItem(*SC, "BP_Cavrnus_BookmarkLoader", TEXT("/CavrnusCVT/CavrnusIntegration/Commands/Bookmarks/AC_Cavrnus_BookmarkLoader.AC_Cavrnus_BookmarkLoader_C"));
					AddSpawnableItem(*SC, "BP_Cavrnus_CropboxLoader", TEXT("/CavrnusCVT/CavrnusIntegration/Commands/CropBox/BP_Cavrnus_CropboxLoader.BP_Cavrnus_CropboxLoader_C"));
					AddSpawnableItem(*SC, "BP_Cavrnus_DatasmithLoader", TEXT("/CavrnusCVT/CavrnusIntegration/Commands/Datasmith/BP_Cavrnus_DatasmithLoader.BP_Cavrnus_DatasmithLoader_C"));
				}
				else
				{
					UE_LOG(LogCavrnusCVTEditor, Warning, TEXT("Failed to cast the actor to SpatialConnector"));
				}
			}
		}
	}
	else
	{
		UE_LOG(LogCavrnusCVTEditor, Warning, TEXT("World context is invalid."));
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
		UE_LOG(LogTemp, Warning, TEXT("FoundGameMode is NOT a subclass of AGameModeBase"));
	}
}

void FCavrnusCVTEditorModule::AddSpawnableItem(ACavrnusSpatialConnector& SC, const FString& Id, const FString& Path)
{
	SC.SpawnableIdentifiers.Add(Id, GetDefaultBlueprint(Path, AActor::StaticClass()));
}

UClass* FCavrnusCVTEditorModule::GetDefaultBlueprint(const FString& Path, UClass* BaseClass)
{
	// Use BP as default value
	UClass* LoadedBlueprintClass = StaticLoadClass(BaseClass, nullptr, *Path, nullptr, LOAD_None, nullptr);
	if (!LoadedBlueprintClass)
	{
		UE_LOG(LogTemp, Error, TEXT("Blueprint asset failed to load from path: %s, base class name: %s"), *Path, *BaseClass->GetName());
	}

	return LoadedBlueprintClass;
}