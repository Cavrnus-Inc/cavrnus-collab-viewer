// // Copyright (c) 2025 Cavrnus. All rights reserved.

#include "UI/Systems/Dialogs/CavrnusBaseDialogWidget.h"

void UCavrnusBaseDialogWidget::CloseDialog()
{
	OnCloseDelegate.Broadcast();
}

void UCavrnusBaseDialogWidget::HookOnClose(const TFunction<void()>& OnCloseCallback)
{
	OnCloseDelegate.AddLambda(OnCloseCallback);
}
