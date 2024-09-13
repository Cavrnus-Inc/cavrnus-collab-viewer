// Copyright(c) Cavrnus. All rights reserved.
#include "UI/CavrnusLoginWidget.h"
#include <Components/EditableTextBox.h>
#include <Components/Button.h>

void UCavrnusLoginWidget::Setup()
{
	LoginButton->OnClicked.AddUniqueDynamic(this, &UCavrnusLoginWidget::OnLoginClicked);
}

void UCavrnusLoginWidget::NativeDestruct()
{
	Super::NativeDestruct();

	LoginButton->OnClicked.RemoveDynamic(this, &UCavrnusLoginWidget::OnLoginClicked);
}

void UCavrnusLoginWidget::OnLoginClicked()
{
	OnLogin.Broadcast(EmailInput->GetText().ToString(), PasswordInput->GetText().ToString());
	RemoveFromParent();
}