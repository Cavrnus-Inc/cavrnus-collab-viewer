// Copyright (c) 2024 Cavrnus. All rights reserved.

#include "UI/CavrnusLoginWidget.h"
#include <Components/Button.h>
#include "CavrnusFunctionLibrary.h"
#include "CavrnusSpatialConnector.h"
#include "CavrnusSpatialConnectorSubSystem.h"
#include "Components/EditableTextBox.h"

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
	const FString Server = UCavrnusFunctionLibrary::GetCavrnusSpatialConnector()->MyServer;
	UCavrnusFunctionLibrary::AuthenticateWithPassword(Server, EmailInput->GetText().ToString(),
	                                                  PasswordInput->GetText().ToString(),
	                                                  [this](const FCavrnusAuthentication&)
	                                                  {
	                                                  }, [](const FString&)
	                                                  {
	                                                  });

	UCavrnusFunctionLibrary::GetCavrnusSpatialConnectorSubSystemProxy()->ShowAuthWidget(true);
	RemoveFromParent();
}