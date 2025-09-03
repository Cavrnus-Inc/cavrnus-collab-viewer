// // Copyright (c) 2025 Cavrnus. All rights reserved.

#include "UI/Widgets/ProjectSetupWizard/CavrnusEditorProjectSetupWizard.h"
#include "Utilities/CavrnusEditorHelpers.h"

FString UCavrnusEditorProjectSetupWizard::PawnPath = TEXT("/CavrnusConnector/Pawns/Local/BP_Cavrnus_Local_Fly.BP_Cavrnus_Local_Fly_C");
FString UCavrnusEditorProjectSetupWizard::GameModePath = TEXT("/CavrnusConnector/CavrnusGameMode.CavrnusGameMode_C");
void UCavrnusEditorProjectSetupWizard::NativeConstruct()
{
	Super::NativeConstruct();
	SetupPawnButton = CreateButton(Required)
	->SetPrimaryText("Assign Synced Pawn")
	->SetSecondaryText("Overrides the current level pawn with the Cavrnus-synced pawn")
	->SetCompletionState(false);
	SetupPawnButton->SetCompletionState(FCavrnusEditorHelpers::IsEditorDefaultPawnSet(GetWorld(), PawnPath));
	SetupPawnButton->OnButtonClicked.AddWeakLambda(this, [this]
		{
			FCavrnusEditorHelpers::SetEditorDefaultPawnByPath(GetWorld(), PawnPath, GameModePath);
			SetupPawnButton->SetCompletionState(true);
		});
}

void UCavrnusEditorProjectSetupWizard::OnPostWorldTriggered()
{
	Super::OnPostWorldTriggered();

	if (SetupPawnButton)
		SetupPawnButton->SetCompletionState(FCavrnusEditorHelpers::IsEditorDefaultPawnSet(GetWorld(), PawnPath)); 
}
