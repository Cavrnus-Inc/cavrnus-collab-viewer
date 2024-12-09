// Copyright (c) 2024 Cavrnus. All rights reserved.

#include "UI/JoinCodeLogin/CavrnusJoinIdLoginWidget.h"

#include "CavrnusFunctionLibrary.h"
#include "CavrnusSpatialConnector.h"
#include "CavrnusSpatialConnectorSubSystem.h"
#include "UI/Components/CavrnusInputField.h"
#include <Components/Button.h>
#include <Components/TextBlock.h>

#pragma region UE Methods
void UCavrnusJoinIdLoginWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	// JoinCode Fields
	JoinCodeInputField->OnInputUpdated.AddDynamic(this, &UCavrnusJoinIdLoginWidget::JoinCodeInputUpdated);
	TryJoinSpaceButton->OnClicked.AddDynamic(this, &UCavrnusJoinIdLoginWidget::TryJoinSpaceButtonClicked);
	CloseButton->OnClicked.AddDynamic(this, &UCavrnusJoinIdLoginWidget::CloseButtonClicked);

	SetCloseButtonVis(false);
	
	// Code Confirm Button
	TryJoinSpaceButton->SetIsEnabled(false);
}

void UCavrnusJoinIdLoginWidget::NativeDestruct()
{
	Super::NativeDestruct();
	
	// JoinCode Fields
	JoinCodeInputField->OnInputUpdated.RemoveDynamic(this, &UCavrnusJoinIdLoginWidget::JoinCodeInputUpdated);
	TryJoinSpaceButton->OnClicked.RemoveDynamic(this, &UCavrnusJoinIdLoginWidget::TryJoinSpaceButtonClicked);
	CloseButton->OnClicked.RemoveDynamic(this, &UCavrnusJoinIdLoginWidget::CloseButtonClicked);
}
#pragma endregion

#pragma region JoinCode InputField
void UCavrnusJoinIdLoginWidget::JoinCodeInputUpdated(const FString&)
{
	JoinCodeInputField->SetMessage("");
	JoinCodeInputField->SetState(UCavrnusInputField::Normal);
	TryJoinSpaceButton->SetIsEnabled(JoinCodeInputField->HasAnyText());
}

void UCavrnusJoinIdLoginWidget::JoinCodeInputCommitted(const FText& Text, const ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		if (!Text.IsEmptyOrWhitespace())
			TryJoinSpace(Text.ToString());
	}
	
	if (CommitMethod == ETextCommit::OnCleared)
		TryJoinSpaceButton->SetIsEnabled(false);
}
#pragma endregion

#pragma region JoinSpaceButton
void UCavrnusJoinIdLoginWidget::TryJoinSpaceButtonClicked()
{
	TryJoinSpace(JoinCodeInputField->GetInputString());
}

void UCavrnusJoinIdLoginWidget::CloseButtonClicked()
{
	OnCloseButtonClicked.Broadcast();
}

void UCavrnusJoinIdLoginWidget::SetJoinSpaceButtonText(const FString& Value)
{
	JoinSpaceButtonText->SetText(FText::FromString(Value));
}

void UCavrnusJoinIdLoginWidget::SetCloseButtonVis(bool bVis)
{
	CloseButton->SetVisibility(bVis ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
}
#pragma endregion

#pragma region JoinLogic
void UCavrnusJoinIdLoginWidget::TryJoinSpace(const FString& JoinId)
{
	if (JoinId.IsEmpty())
		return;

	SetVisibility(ESlateVisibility::Hidden);
	UCavrnusFunctionLibrary::JoinSpace(JoinId.TrimStartAndEnd(),
	                                   [this](const FCavrnusSpaceConnection&)
	                                   {
	                                   	RemoveFromParent();
	                                   }, [this](const FString&)
	                                   {
	                                   	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	                                   	UCavrnusFunctionLibrary::GetCavrnusSpatialConnectorSubSystemProxy()->ShowLoadingWidget(false);

	                                   	TryJoinSpaceButton->SetIsEnabled(false);
										JoinCodeInputField->Reset();
	                                   	JoinCodeInputField->SetMessage("Invalid JoinId!");
	                                   	JoinCodeInputField->SetState(UCavrnusInputField::Invalid);
	                                   });
}
#pragma endregion