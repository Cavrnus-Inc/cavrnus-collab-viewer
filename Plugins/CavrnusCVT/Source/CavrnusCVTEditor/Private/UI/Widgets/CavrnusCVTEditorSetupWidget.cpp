// Copyright (c) 2025 Cavrnus. All rights reserved.

#include "UI/Widgets/CavrnusCVTEditorSetupWidget.h"
#include "LevelSetup/CavrnusCVTLevelSetupHelper.h"
#include "UI/Helpers/CavrnusWidgetFactory.h"

void UCavrnusCVTEditorSetupWidget::NativeConstruct()
{
	Super::NativeConstruct();

	const auto CollButton = CreateButton(Required)
		->SetPrimaryText("Setup Project Settings")
		->SetSecondaryText("This will add missing collision profiles to the DefaultEngine.ini file. Unreal Editor will restart automatically afterwards.")
		->SetCompletionState(false);
	CollButton->SetCompletionState(FCavrnusCVTLevelSetupHelper::HasSetupCollisionProfiles());
	CollButton->OnButtonClicked.AddWeakLambda(this, [CollButton]
		{
			FCavrnusCVTLevelSetupHelper::AddCollisionProfiles();
			CollButton->SetCompletionState(true); // just making assumption here. May need to be more robust.
		});
	
	const auto GameModeButton = CreateButton(Required)
		->SetPrimaryText("Set GameMode")
		->SetSecondaryText("Sets the current Level's GameMode Overide to use the Collab Viewer's")
		->SetCompletionState(false);
	GameModeButton->SetCompletionState(FCavrnusCVTLevelSetupHelper::HasGameMode());
	GameModeButton->OnButtonClicked.AddWeakLambda(this, [GameModeButton]
		{
			FCavrnusCVTLevelSetupHelper::SetupGameMode();
			GameModeButton->SetCompletionState(true);
		});
	
	const auto CvtManagerButton = CreateButton(Required)
	->SetPrimaryText("Add Collab Viewer Manager Actor")
	->SetSecondaryText("Adds necessary subsystems to current level")
	->SetCompletionState(false);
	CvtManagerButton->SetCompletionState(FCavrnusCVTLevelSetupHelper::HasCvtManager());
	CvtManagerButton->OnButtonClicked.AddWeakLambda(this, [CvtManagerButton]
		{
			FCavrnusCVTLevelSetupHelper::AddCVTManagerToScene();
			CvtManagerButton->SetCompletionState(true);
		});

	const auto StaticMeshButton = CreateButton(Optional)
	->SetPrimaryText("Set actors static mesh")
	->SetSecondaryText("Convert All Level Actors to Movable (Performance Cost)")
	->SetCompletionState(false)
	->ShowCheckMarkContainer(false);
	StaticMeshButton->OnButtonClicked.AddWeakLambda(this, []
		{
			FCavrnusCVTLevelSetupHelper::ConvertStaticMeshActors();
		});

	// Simple check to see if anything has changed
	WorldDelegate = FWorldDelegates::OnPostWorldInitialization.AddWeakLambda(this, [CollButton, GameModeButton, CvtManagerButton](const UWorld* World, const UWorld::InitializationValues)
	{
		if (World && World->WorldType == EWorldType::Editor)
		{
			UE_LOG(LogTemp, Log, TEXT("World initialized: %s"), *World->GetName());

			GameModeButton->SetCompletionState(FCavrnusCVTLevelSetupHelper::HasGameMode());
			CvtManagerButton->SetCompletionState(FCavrnusCVTLevelSetupHelper::HasCvtManager());
			CollButton->SetCompletionState(FCavrnusCVTLevelSetupHelper::HasSetupCollisionProfiles());
		}
	});
}

void UCavrnusCVTEditorSetupWidget::NativeDestruct()
{
	Super::NativeDestruct();

	FWorldDelegates::OnPostWorldInitialization.Remove(WorldDelegate);
	WorldDelegate.Reset();

	Buttons.Empty();
}

UCavrnusCvtSetupButton* UCavrnusCVTEditorSetupWidget::CreateButton(const EButtonTypeEnum& ButtonType)
{
	if (SetupButtonBlueprint)
	{
		auto* Button = FCavrnusWidgetFactory::CreateUserWidget<UCavrnusCvtSetupButton>(SetupButtonBlueprint, GetWorld());

		switch (ButtonType)
		{
		case Required:
			if (ButtonContainerRequired) ButtonContainerRequired->AddChildToVerticalBox(Button);
			break;
		case Optional:
			if (ButtonContainerOptional) ButtonContainerOptional->AddChildToVerticalBox(Button);
			break;
		}
		
		Buttons.Add(Button);

		return Button;
	}

	return nullptr;
}
