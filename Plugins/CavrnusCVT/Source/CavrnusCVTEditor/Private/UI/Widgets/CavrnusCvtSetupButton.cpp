// Copyright (c) 2025 Cavrnus. All rights reserved.

#include "UI/Widgets/CavrnusCvtSetupButton.h"
#include "Components/Border.h"

UCavrnusCvtSetupButton* UCavrnusCvtSetupButton::SetPrimaryText(const FString& InPrimaryText)
{
	if (PrimaryText)
		PrimaryText->SetText(FText::FromString(InPrimaryText));
	
	return this;
}

UCavrnusCvtSetupButton* UCavrnusCvtSetupButton::SetSecondaryText(const FString& InPrimaryText)
{
	if (SecondaryText)
		SecondaryText->SetText(FText::FromString(InPrimaryText));
	
	return this;
}

UCavrnusCvtSetupButton* UCavrnusCvtSetupButton::SetCompletionState(const bool bIsComplete)
{
	if (Checkmark)
	{
		ShowCheckMarkContainer(true);
		Checkmark->SetVisibility(bIsComplete ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Hidden);
	}

	return this;
}

UCavrnusCvtSetupButton* UCavrnusCvtSetupButton::ShowCheckMarkContainer(const bool bShow)
{
	if (CheckmarkContainer)
		CheckmarkContainer->SetVisibility(bShow ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);

	return this;
}

void UCavrnusCvtSetupButton::NativePreConstruct()
{
	Super::NativePreConstruct();

	ShowCheckMarkContainer(false);
	SetCompletionState(false);
}