// // Copyright (c) 2025 Cavrnus. All rights reserved.

#include "UI/Systems/Messages/CavrnusBaseUIMessageWidget.h"

void UCavrnusBaseUIMessageWidget::NativeDestruct()
{
	Super::NativeDestruct();
	
	if (CloseButton)
	{
		CloseButton->OnButtonClicked.Remove(CloseButtonHandle);
		CloseButtonHandle.Reset();
	}
}

UCavrnusBaseUIMessageWidget* UCavrnusBaseUIMessageWidget::HookOnClose(const TFunction<void()>& OnClose)
{
	if (CloseButton)
		CloseButtonHandle = CloseButton->OnButtonClicked.AddWeakLambda(this, [OnClose] { OnClose(); });

	return this;
}