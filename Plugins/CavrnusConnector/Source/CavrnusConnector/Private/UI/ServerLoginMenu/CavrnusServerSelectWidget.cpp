// Copyright (c) 2024 Cavrnus. All rights reserved.

#include "UI/ServerLoginMenu/CavrnusServerSelectWidget.h"
#include "CavrnusFunctionLibrary.h"
#include "CavrnusSpatialConnector.h"
#include "CavrnusSpatialConnectorSubSystem.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"

void UCavrnusSeverSelectWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	ProceedButton->OnClicked.AddDynamic(this, &UCavrnusSeverSelectWidget::ProceedButtonClicked);
	ServerTextBox->OnTextChanged.AddDynamic(this, &UCavrnusSeverSelectWidget::ServerTextBoxUpdated);
	
	ProceedButton->SetIsEnabled(false);
}

void UCavrnusSeverSelectWidget::NativeDestruct()
{
	Super::NativeDestruct();
	
	ProceedButton->OnClicked.RemoveDynamic(this, &UCavrnusSeverSelectWidget::ProceedButtonClicked);
	ServerTextBox->OnTextChanged.RemoveDynamic(this, &UCavrnusSeverSelectWidget::ServerTextBoxUpdated);
}

void UCavrnusSeverSelectWidget::ProceedButtonClicked()
{
	if (ACavrnusSpatialConnector* Csc = UCavrnusFunctionLibrary::GetCavrnusSpatialConnector())
		Csc->MyServer = ServerTextBox->GetText().ToString();
	
	if (UCavrnusSpatialConnectorSubSystemProxy* Csp = UCavrnusFunctionLibrary::GetCavrnusSpatialConnectorSubSystemProxy())
		Csp->BeginAuthenticationProcess();

	RemoveFromParent();
}

void UCavrnusSeverSelectWidget::ServerTextBoxUpdated(const FText& Text)
{
	ProceedButton->SetIsEnabled(!Text.IsEmptyOrWhitespace());
}