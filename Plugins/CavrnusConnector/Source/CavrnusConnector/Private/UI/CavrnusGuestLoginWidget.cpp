// Copyright(c) Cavrnus. All rights reserved.
#include "UI/CavrnusGuestLoginWidget.h"
#include <Components/EditableTextBox.h>
#include <Components/Button.h>

void UCavrnusGuestLoginWidget::Setup()
{
	LoginButton->OnClicked.AddUniqueDynamic(this, &UCavrnusGuestLoginWidget::OnLoginClicked);
}

void UCavrnusGuestLoginWidget::NativeDestruct()
{
	Super::NativeDestruct();
	
	LoginButton->OnClicked.RemoveDynamic(this, &UCavrnusGuestLoginWidget::OnLoginClicked);
}

void UCavrnusGuestLoginWidget::OnLoginClicked()
{
	OnLogin.Broadcast(GuestUsernameInput->GetText().ToString());
}