// Copyright (c) 2025 Cavrnus. All rights reserved.

#include "LevelSetup/CavrnusCVTLevelSetupHelper.h"

#include "CavrnusCVTEditorModule.h"
#include "CavrnusCVTManager.h"
#include "CavrnusKeyValueStore.h"
#include "EngineUtils.h"
#include "Engine/StaticMeshActor.h"
#include "GameFramework/GameModeBase.h"

TArray<FString> FCavrnusCVTLevelSetupHelper::CollisionProfiles =
{
	"Annotation",
	"AnnotationStroke",
	"SnapShot",
	"DimenssionTool",
	"Gizmo",
	"Xrayed",
	"UI_A"
};

void FCavrnusCVTLevelSetupHelper::AddCollisionProfiles()
{
	for (const FString& Profile : CollisionProfiles)
		RegisterCollisionChannelIfMissing(Profile);
}

bool FCavrnusCVTLevelSetupHelper::HasSetupCollisionProfiles()
{
	const FString ConfigPath = FPaths::ProjectConfigDir() / TEXT("DefaultEngine.ini");
	const FString SectionHeader = TEXT("[/Script/Engine.CollisionProfile]");

	FString FileContents;
	if (!FFileHelper::LoadFileToString(FileContents, *ConfigPath))
	{
		UE_LOG(LogTemp, Error, TEXT("[FCavrnusCollisionProfileHelper::HasSetupCollisionProfiles] Failed to read DefaultEngine.ini"));
		return false;
	}
	
	for (const FString& Profile : CollisionProfiles)
	{
		if (!FileContents.Contains(FString::Printf(TEXT("Name=\"%s\""), *Profile)))
		{
			UE_LOG(LogTemp, Log, TEXT("Collision channel '%s' already exists."), *Profile);
			return false;
		}
	}

	return true;
}

void FCavrnusCVTLevelSetupHelper::ConvertStaticMeshActors()
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

void FCavrnusCVTLevelSetupHelper::AddCVTManagerToScene()
{
	if (GEditor)
	{
		if (UWorld* World = GEditor->GetEditorWorldContext().World())
		{
			if (!HasCvtManager())
			{
				if (UClass* Found = GetDefaultBlueprint(
					TEXT("/CavrnusCVT/CavrnusIntegration/Commands/A_CavrnusCVTManager.A_CavrnusCVTManager_C"),
					AActor::StaticClass()))
				{
					World->SpawnActor(Found);
				}
			}
		}
	}
}

bool FCavrnusCVTLevelSetupHelper::HasCvtManager()
{
	if (GEditor)
	{
		if (UWorld* World = GEditor->GetEditorWorldContext().World())
		{
			for (TActorIterator<AActor> It(World, ACavrnusCVTManager::StaticClass()); It;)
				return true;
		}
	}

	return false;
}

void FCavrnusCVTLevelSetupHelper::SetupGameMode()
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

bool FCavrnusCVTLevelSetupHelper::HasGameMode()
{
	if (UWorld* World = GEditor->GetEditorWorldContext().World())
	{
		if (AWorldSettings* WorldSettings = World->GetWorldSettings())
		{
			UClass* FoundGameMode = GetDefaultBlueprint(TEXT("/CavrnusCVT/CollaborativeViewer/Blueprints/GameMode/BP_CollaborativeViewer_GameMode.BP_CollaborativeViewer_GameMode_C"),
				AGameModeBase::StaticClass());

			return (WorldSettings->DefaultGameMode == FoundGameMode);
		}
	}

	return false;
}

void FCavrnusCVTLevelSetupHelper::RegisterCollisionChannelIfMissing(const FString& ChannelName)
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

UClass* FCavrnusCVTLevelSetupHelper::GetDefaultBlueprint(const FString& Path, UClass* BaseClass)
{
	// Use BP as default value
	UClass* LoadedBlueprintClass = StaticLoadClass(BaseClass, nullptr, *Path, nullptr, LOAD_None, nullptr);
	if (!LoadedBlueprintClass)
		UE_LOG(LogTemp, Error, TEXT("Blueprint asset failed to load from path: %s, base class name: %s"), *Path, *BaseClass->GetName());

	return LoadedBlueprintClass;
}


FString FCavrnusCVTLevelSetupHelper::GetLevelGUID()
{
	if (const UWorld* World = GEditor->GetEditorWorldContext().World())
	{
		const FGuid PackageGuid = World->GetPackage()->GetPersistentGuid();
		UE_LOG(LogTemp, Log, TEXT("Map GUID: %s"), *PackageGuid.ToString());
		
		return PackageGuid.ToString();
	}
	
	return "";
}
