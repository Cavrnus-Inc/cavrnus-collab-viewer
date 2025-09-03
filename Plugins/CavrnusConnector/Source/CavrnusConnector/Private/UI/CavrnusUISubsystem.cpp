// // Copyright (c) 2025 Cavrnus. All rights reserved.

#include "UI/CavrnusUISubsystem.h"

#include "CavrnusConnectorModule.h"
#include "UI/CavrnusUIConfigAsset.h"
#include "UI/Helpers/CavrnusWidgetFactory.h"
#include "UI/Styles/CavrnusUIThemeManager.h"
#include "UI/Systems/Dialogs/CavrnusDialogSystem.h"
#include "UI/Systems/Displayers/CavrnusDesktopCanvasWidgetDisplayer.h"
#include "UI/Systems/Loaders/CavrnusUILoaderSystem.h"
#include "UI/Systems/Messages/CavrnusScopedMessages.h"
#include "UI/Systems/Popups/Types/CavrnusPopupSystem.h"
#include "UI/Systems/RawWidgetHost/CavrnusRawWidgetHost.h"
#include "UI/Systems/Scrims/CavrnusUIScrimSystem.h"

class UCavrnusConfirmationDialog;
class UCavrnusServerStatusToastWidget;
class UCavrnusUIConfigAsset;

bool UCavrnusUISubsystem::HasUIInit = false;
TMulticastDelegate<void()> UCavrnusUISubsystem::OnUIInit;

void UCavrnusUISubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UIConfig = FCavrnusWidgetFactory::LoadAssetFromPath<UCavrnusUIConfigAsset>("/CavrnusConnector/UI/Data/DA_CavrnusUIConfig.DA_CavrnusUIConfig");
	if (!UIConfig)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load CavrnusUIConfig Asset"));
		return;
	}
				
	MenuLookup = UIConfig->MenuLookupObject.LoadSynchronous();
	if (!MenuLookup)
	{
		UE_LOG(LogCavrnusConnector, Error, TEXT("Failed to load MenuLookupObject Asset"));
		return;
	}
}

void UCavrnusUISubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	if (UIConfig == nullptr)
	{
		UE_LOG(LogCavrnusConnector, Error, TEXT("[UCavrnusUISubsystem::OnWorldBeginPlay] Failed to load MenuLookupObject Asset"));
		return;
	}

	CachedThemeManager = NewObject<UCavrnusUIThemeManager>(this);
	CachedThemeManager->Initialize();
			
	auto* CanvasDisplayer = FCavrnusWidgetFactory::CreateUserWidget<UCavrnusDesktopCanvasWidgetDisplayer>(UIConfig->CanvasDisplayer, &InWorld);
	CanvasDisplayer->Setup(MenuLookup);

	GenericWidgetDisplayerSystem = NewObject<UCavrnusRawWidgetHost>(this);
	GenericWidgetDisplayerSystem->Initialize(this, CanvasDisplayer);
				
	ScrimSystem = NewObject<UCavrnusUIScrimSystem>(this);
	ScrimSystem->Initialize(MenuLookup, CanvasDisplayer);

	PopupSystem = NewObject<UCavrnusPopupSystem>(this);
	PopupSystem->Initialize(MenuLookup, CanvasDisplayer);
				
	// Scoped messages object is to hold eventual other types of Message systems
	ScopedMessages = NewObject<UCavrnusScopedMessages>(this);
	ScopedMessages->Initialize(MenuLookup, CanvasDisplayer);

	LoaderSystem = NewObject<UCavrnusUILoaderSystem>(this);
	LoaderSystem->Initialize(MenuLookup, CanvasDisplayer);

	DialogSystem = NewObject<UCavrnusDialogSystem>(this);
	DialogSystem->Initialize(MenuLookup, CanvasDisplayer);

	HasUIInit = true;
	if (OnUIInit.IsBound())
	{
		OnUIInit.Broadcast();
		OnUIInit.Clear();
	}
}

void UCavrnusUISubsystem::Deinitialize()
{
	Super::Deinitialize();

	if (PopupSystem)
		PopupSystem->Teardown();
	
	PopupSystem = nullptr;
	ScopedMessages = nullptr;
	HasUIInit = false;
}

void UCavrnusUISubsystem::AwaitUIInit(const TFunction<void()>& OnCallback)
{
	if (HasUIInit)
	{
		OnCallback();
		return;
	}

	OnUIInit.AddLambda(OnCallback);
}
