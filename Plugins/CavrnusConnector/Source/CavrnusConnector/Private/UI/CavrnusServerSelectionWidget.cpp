// Copyright(c) Cavrnus. All rights reserved.
#include "UI/CavrnusServerSelectionWidget.h"
#include <Components/EditableTextBox.h>
#include <Components/Button.h>

void UCavrnusServerSelectionWidget::Setup()
{
	ProceedButton->OnClicked.AddUniqueDynamic(this, &UCavrnusServerSelectionWidget::OnProceedClicked);
}

void UCavrnusServerSelectionWidget::NativeDestruct()
{
	Super::NativeDestruct();

	ProceedButton->OnClicked.RemoveDynamic(this, &UCavrnusServerSelectionWidget::OnProceedClicked);
}

void UCavrnusServerSelectionWidget::OnProceedClicked()
{
	OnProceed.Broadcast(DomainInput->GetText().ToString());
	RemoveFromParent();
}