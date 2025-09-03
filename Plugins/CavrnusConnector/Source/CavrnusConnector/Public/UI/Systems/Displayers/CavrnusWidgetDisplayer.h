// // Copyright (c) 2025 Cavrnus. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Blueprint/UserWidget.h"
#include "CavrnusWidgetDisplayer.generated.h"

struct FCavrnusScrimOptions;
class UCavrnusBaseScrimWidget;
struct FCavrnusUILoaderOptions;
class UCavrnusBaseUILoaderWidget;
class UCavrnusWidgetBlueprintLookup;
class UCavrnusBaseUserWidget;
struct FCavrnusDialogOptions;
class UCavrnusBaseToastMessageWidget;
class UCavrnusBasePopupWidget;
class UCavrnusBaseDialogWidget;
struct FCavrnusToastMessageOptions;
struct FCavrnusPopupOptions;

USTRUCT(BlueprintType)
struct FCavrnusBaseDisplayOptions
{
	GENERATED_BODY()
	
	int32 ZOrder = 0;

	FCavrnusBaseDisplayOptions()
	: ZOrder(0)
	{}
};

// This class does not need to be modified.
UINTERFACE()
class UCavrnusWidgetDisplayer : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class CAVRNUSCONNECTOR_API ICavrnusWidgetDisplayer
{
	GENERATED_BODY()

public:
	virtual void Setup(UCavrnusWidgetBlueprintLookup* InBlueprintLookup = nullptr) = 0;

	// Specific display types live here! This will grow as more systems are added...better than having an enum
	virtual void DisplayRawWidget(const FGuid& Id, UUserWidget* InWidgetToDisplay);
	virtual void DisplayScrimWidget(UCavrnusBaseScrimWidget* InWidgetToDisplay, const FCavrnusScrimOptions& Options, const TFunction<void(UCavrnusBaseScrimWidget*)>& OnWidgetClosed = nullptr);
	virtual void DisplayDialogWidget(UCavrnusBaseDialogWidget* InWidgetToDisplay, const FCavrnusDialogOptions& Options, const TFunction<void(UCavrnusBaseDialogWidget*)>& OnWidgetClosed = nullptr);
	virtual void DisplayPopupWidget(UCavrnusBasePopupWidget* InWidgetToDisplay, const FCavrnusPopupOptions& Options, const TFunction<void(UCavrnusBasePopupWidget*)>& OnWidgetClosed = nullptr);
	virtual void DisplayToastMessageWidget(UCavrnusBaseToastMessageWidget* InWidgetToDisplay, const FCavrnusToastMessageOptions& Options, const TFunction<void(UCavrnusBaseToastMessageWidget*)>& OnWidgetClosed = nullptr);
	virtual void DisplayLoaderWidget(UCavrnusBaseUILoaderWidget* InWidgetToDisplay, const FCavrnusUILoaderOptions& Options, const TFunction<void(UCavrnusBaseUILoaderWidget*)>& OnWidgetClosed = nullptr);
	
	virtual void RemoveWidget(const FGuid& Id) = 0;
	virtual void RemoveAll() = 0;
};