// // Copyright (c) 2025 Cavrnus. All rights reserved.

#include "UI/Systems/Displayers/CavrnusWidgetDisplayer.h"

void ICavrnusWidgetDisplayer::DisplayDialogWidget(UCavrnusBaseDialogWidget* InUserWidget,
                                                  const FCavrnusDialogOptions& Options,
                                                  const TFunction<void(UCavrnusBaseDialogWidget*)>& OnWidgetClosed)
{
	UE_LOG(LogTemp, Warning, TEXT("Attempting to DisplayDialogWidget!"));
}

void ICavrnusWidgetDisplayer::DisplayPopupWidget(UCavrnusBasePopupWidget* InUserWidget,
                                                 const FCavrnusPopupOptions& Options,
                                                 const TFunction<void(UCavrnusBasePopupWidget*)>& OnWidgetClosed)
{
	UE_LOG(LogTemp, Warning, TEXT("Attempting to DisplayPopupWidget!"));
}

void ICavrnusWidgetDisplayer::DisplayToastMessageWidget(UCavrnusBaseToastMessageWidget* InUserWidget,
                                                        const FCavrnusToastMessageOptions& Options,
                                                        const TFunction<void(UCavrnusBaseToastMessageWidget*)>&
                                                        OnWidgetClosed)
{
	UE_LOG(LogTemp, Warning, TEXT("Attempting to DisplayToastMessageWidget!"));
}

void ICavrnusWidgetDisplayer::DisplayLoaderWidget(
													UCavrnusBaseUILoaderWidget* InWidgetToDisplay,
													const FCavrnusUILoaderOptions& Options,
													const TFunction<void(UCavrnusBaseUILoaderWidget*)>& OnWidgetClosed)
{
	UE_LOG(LogTemp, Warning, TEXT("Attempting to DisplayLoaderWidget!"));
}

void ICavrnusWidgetDisplayer::DisplayRawWidget(const FGuid& Id, UUserWidget* InWidgetToDisplay)
{
	UE_LOG(LogTemp, Warning, TEXT("Attempting to DisplayRawWidget!"));
}

void ICavrnusWidgetDisplayer::DisplayScrimWidget(
	UCavrnusBaseScrimWidget* InWidgetToDisplay,
	const FCavrnusScrimOptions& Options,
	const TFunction<void(UCavrnusBaseScrimWidget*)>& OnWidgetClosed)
{
	UE_LOG(LogTemp, Warning, TEXT("Attempting to DisplayScrimWidget!"));
}
