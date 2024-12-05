// Copyright (c) 2024 Cavrnus. All rights reserved.

#include "UI/CavrnusGuestLoginWidget.h"
#include <Components/EditableTextBox.h>
#include <Components/Button.h>

#include "CavrnusFunctionLibrary.h"
#include "CavrnusSpatialConnector.h"
#include "CavrnusSpatialConnectorSubSystem.h"

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
	const FString Server = UCavrnusFunctionLibrary::GetCavrnusSpatialConnector()->MyServer;
	UCavrnusFunctionLibrary::AuthenticateAsGuest(Server, GuestUsernameInput->GetText().ToString(),
	                                             [this](const FCavrnusAuthentication&)
	                                             {
	                                             },
	                                             [](const FString&)
	                                             {
	                                             });

	UCavrnusFunctionLibrary::GetCavrnusSpatialConnectorSubSystemProxy()->ShowAuthWidget(true);
	RemoveFromParent();
}