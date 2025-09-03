// // Copyright (c) 2025 Cavrnus. All rights reserved.

#include "CavrnusSubsystem.h"
#include "CavrnusConnectorSettings.h"

#include "Managers/CavrnusEditorAuthenticationManager.h"
#include "Managers/Login/CavrnusLoginManager.h"

#include "Engine/Engine.h"
#include "Engine/World.h"

#if WITH_EDITOR
#include "Editor.h"
#include "Editor/EditorEngine.h"
#endif

UCavrnusSubsystem* UCavrnusSubsystem::Get()
{
	if (GEngine)
		return GEngine->GetEngineSubsystem<UCavrnusSubsystem>();
	
	return nullptr;
}

void UCavrnusSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UE_LOG(LogTemp, Log, TEXT("CavrnusSubsystem initialized"));

	CachedAuthManager = NewObject<UCavrnusEditorAuthenticationManager>();
	CachedAuthManager->Initialize();



#if WITH_EDITOR
	FEditorDelegates::BeginPIE.AddLambda([this](bool)
	{
		UE_LOG(LogTemp, Log, TEXT("CavrnusSubsystem: BeginPIE called!"));
		if (GetAuthManager()->HasEditorAuthenticated())
			GetLoginManager()->DoPieLogin();
		else
		{
			if (GetSettings()->ConnectOnStart)
				GetLoginManager()->DoPluginSettingsLogin();
		}
	});
#else
	WorldHandle =  FWorldDelegates::OnPostWorldInitialization.AddLambda(
	[this](const UWorld* World, const UWorld::InitializationValues&)
	{
		// This World check doesn't solve the double fire problem
		UE_LOG(LogTemp, Log, TEXT("OnPostWorldInitialization called for CavrnusSubsystem!"));
		if (!World || World->WorldType != EWorldType::Game || !World->IsGameWorld())
		{
			UE_LOG(LogTemp, Log, TEXT("CavrnusSubsystem:Initialize ignoring non-game world.  Should run again"));
			return;
		}
		// OnPostWorldInitialization gets called twice in builds.
		UGameInstance* GameInstance = World->GetGameInstance();
		if (!GameInstance)
		{
			UE_LOG(LogTemp, Log, TEXT("No Game Instance.  Returning"));
			return;
		}
		if (const auto* Settings = GetSettings())
		{
			UE_LOG(LogTemp, Log, TEXT("Settings : ConnectOnStart: %s, AuthMethod: %s"),
				Settings->ConnectOnStart ? TEXT("true") : TEXT("false"),
				*UEnum::GetValueAsString(Settings->AuthMethod));
			if (Settings->ConnectOnStart)
				GetLoginManager()->DoPluginSettingsLogin();
		}
	});
#endif
}

void UCavrnusSubsystem::Deinitialize()
{
	Super::Deinitialize();
	UE_LOG(LogTemp, Log, TEXT("CavrnusSubsystem deinitialized"));

	if (CachedLoginManager)
		CachedLoginManager->RemoveFromRoot();

	if (CachedAuthManager)
		CachedAuthManager->Teardown();

	CachedAuthManager = nullptr;
	CachedLoginManager = nullptr;

	FWorldDelegates::OnPostWorldInitialization.Remove(WorldHandle);
	WorldHandle.Reset();
}

UCavrnusLoginManager* UCavrnusSubsystem::GetLoginManager()
{
	if (CachedLoginManager == nullptr)
	{
#if WITH_EDITOR
		CachedLoginManager = NewObject<UCavrnusLoginManager>(GetMutableDefault<UCavrnusSubsystem>());
#else
		CachedLoginManager = NewObject<UCavrnusLoginManager>(const_cast<UCavrnusSubsystem*>(GetDefault<UCavrnusSubsystem>())); 
#endif
		CachedLoginManager->AddToRoot();
		UE_LOG(LogTemp, Log, TEXT("LoginManager lazily created"));
	}
	
	return CachedLoginManager;
}
