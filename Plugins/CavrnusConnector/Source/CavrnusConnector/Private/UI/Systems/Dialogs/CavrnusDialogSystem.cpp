// // Copyright (c) 2025 Cavrnus. All rights reserved.

#include "UI/Systems/Dialogs/CavrnusDialogSystem.h"

#include "UI/Helpers/CavrnusWidgetFactory.h"
#include "UI/Systems/AssetLookup/CavrnusWidgetBlueprintLookup.h"
#include "UI/Systems/Dialogs/CavrnusBaseDialogWidget.h"

void UCavrnusDialogSystem::Initialize(UCavrnusWidgetBlueprintLookup* InLookup, ICavrnusWidgetDisplayer* InDisplayer)
{
	Lookup = InLookup;
	
	DisplayerObj = Cast<UObject>(InDisplayer);
	Displayer = InDisplayer;
}

void UCavrnusDialogSystem::Close(UCavrnusBaseUserWidget* WidgetToClose)
{
	if (WidgetToClose == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("[UCavrnusDialogSystem::Close] WidgetToClose is null!"))
		return;
	}

	if (auto* FoundDialog = Cast<UCavrnusBaseDialogWidget>(WidgetToClose))
	{
		Displayer->RemoveWidget(FoundDialog->GetId());
		
		Dialogs.Remove(FoundDialog);
		DialogMap.Remove(FoundDialog->GetId());
	}
}

void UCavrnusDialogSystem::CloseAll()
{
	if (Dialogs.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("[UCavrnusDialogSystem::CloseAll] There are currently no dialogs open!"))
		return;
	}

	Displayer->RemoveAll();
	
	Dialogs.Empty();
	DialogMap.Empty();
}

void UCavrnusDialogSystem::Teardown()
{
	Displayer->RemoveAll();
}

UCavrnusBaseDialogWidget* UCavrnusDialogSystem::CreateInternal(const UClass* Type, const FCavrnusDialogOptions& Options)
{
	CloseAll(); // enforcing one open dialog at a time!
	
	const auto FoundBlueprint = Lookup->GetDialogBlueprint(Type);
	if (FoundBlueprint == nullptr)
		return nullptr;

	if (UCavrnusBaseDialogWidget* DialogWidget = FCavrnusWidgetFactory::CreateUserWidget<UCavrnusBaseDialogWidget>(FoundBlueprint, GetWorld()))
	{
		const auto Id = FGuid::NewGuid();
		DialogWidget->SetId(Id);

		DialogWidget->HookOnClose([this, DialogWidget] { Close(DialogWidget); });
		
		DialogMap.Add(Id, DialogWidget);
		Displayer->DisplayDialogWidget(DialogWidget, Options, [this](UCavrnusBaseDialogWidget* WidgetToClose)
		{
			if (WidgetToClose)
				Close(WidgetToClose);
		});

		return DialogWidget;
	}

	return nullptr;
}
