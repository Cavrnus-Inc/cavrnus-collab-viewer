// Copyright (c) 2025 Cavrnus. All rights reserved.

#include "UI/CavrnusCVTEditorUIManager.h"
#include "UI/Helpers/CavrnusWidgetFactory.h"
#include "UI/Widgets/CavrnusCVTEditorSetupWidget.h"
#include "UI/Widgets/CavrnusMainEditorPanelWidget.h"

void UCavrnusCVTEditorUIManager::Initialize()
{
	UCavrnusMainEditorPanelWidget::OnConstructed.AddLambda([](UCavrnusMainEditorPanelWidget* Widget)
	{
		if (auto* SetupWidget = FCavrnusWidgetFactory::CreateUserWidgetFromPath<UCavrnusCVTEditorSetupWidget>(
			"/CavrnusCVT/CavrnusIntegration/UI/Editor/Menus/WBP_CavrnusCVT_EditorSetupWidget.WBP_CavrnusCVT_EditorSetupWidget_C",
			Widget->GetWorld()))
			Widget->InjectWidget(SetupWidget);
	});
}

void UCavrnusCVTEditorUIManager::Teardown()
{
}
