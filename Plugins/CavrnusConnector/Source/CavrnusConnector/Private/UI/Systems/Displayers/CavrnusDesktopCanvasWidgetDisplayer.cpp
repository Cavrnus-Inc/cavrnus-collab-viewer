// // Copyright (c) 2025 Cavrnus. All rights reserved.

#include "UI/Systems/Displayers/CavrnusDesktopCanvasWidgetDisplayer.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/PanelWidget.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "UI/Helpers/CavrnusWidgetFactory.h"
#include "UI/Systems/AssetLookup/CavrnusWidgetBlueprintLookup.h"
#include "UI/Systems/Containers/CavrnusPopupContainer.h"
#include "UI/Systems/Dialogs/CavrnusBaseDialogWidget.h"
#include "UI/Systems/Loaders/CavrnusBaseUILoaderWidget.h"
#include "UI/Systems/Messages/ToastMessages/CavrnusBaseToastMessageWidget.h"
#include "UI/Systems/Popups/CavrnusBasePopupWidget.h"
#include "UI/Systems/Scrims/CavrnusBaseScrimWidget.h"

class UOverlaySlot;
class UOverlay;

void UCavrnusDesktopCanvasWidgetDisplayer::Setup(UCavrnusWidgetBlueprintLookup* InBlueprintLookup)
{
	BpLookup = InBlueprintLookup;
	DisplayedWidgets.Empty();

	bIsInViewport = false;
	ShouldAddToViewport();
}

void UCavrnusDesktopCanvasWidgetDisplayer::NativeConstruct()
{
	bIsInViewport = true;
}

void UCavrnusDesktopCanvasWidgetDisplayer::NativeDestruct()
{
	Super::NativeDestruct();

	bIsInViewport = false;
}

void UCavrnusDesktopCanvasWidgetDisplayer::DisplayRawWidget(const FGuid& Id, UUserWidget* InWidgetToDisplay)
{
	ShouldAddToViewport();
	if (UOverlaySlot* USlot = Cast<UOverlaySlot>(RawWidgets->AddChildToOverlay(InWidgetToDisplay)))
	{
		USlot->SetHorizontalAlignment(HAlign_Fill);
		USlot->SetVerticalAlignment(VAlign_Fill);

		DisplayedWidgets.Add(Id, InWidgetToDisplay);
	}
}

void UCavrnusDesktopCanvasWidgetDisplayer::DisplayScrimWidget(UCavrnusBaseScrimWidget* InWidgetToDisplay,
                                                              const FCavrnusScrimOptions& Options, const TFunction<void(UCavrnusBaseScrimWidget*)>& OnWidgetClosed)
{
	ShouldAddToViewport();
	if (UOverlaySlot* USlot = Cast<UOverlaySlot>(Scrims->AddChildToOverlay(InWidgetToDisplay)))
	{
		USlot->SetHorizontalAlignment(HAlign_Fill);
		USlot->SetVerticalAlignment(VAlign_Fill);

		DisplayedWidgets.Add(InWidgetToDisplay->GetId(), InWidgetToDisplay);
	}
}

void UCavrnusDesktopCanvasWidgetDisplayer::DisplayPopupWidget(
	UCavrnusBasePopupWidget* InUserWidget,
	const FCavrnusPopupOptions& Options,
	const TFunction<void(UCavrnusBasePopupWidget*)>& OnWidgetClosed)
{
	UUserWidget* WidgetToDisplay = InUserWidget;

	// Wrap the Popup w/ container if found!
	if (BpLookup)
	{
		TSubclassOf<UCavrnusPopupContainer> ContainerBp = static_cast<TSubclassOf<UCavrnusPopupContainer>>(BpLookup->GetContainerBlueprint(UCavrnusPopupContainer::StaticClass()));
		if (auto* ContainerWidget = FCavrnusWidgetFactory::CreateUserWidget<UCavrnusPopupContainer>(ContainerBp, GetWorld()))
		{
			ContainerWidget->SetTitle(InUserWidget->GetTitle());
			ContainerWidget->SetWidgetContent(InUserWidget);
			ContainerWidget->HookOnCloseButton([InUserWidget, OnWidgetClosed]
			{
				OnWidgetClosed(InUserWidget);
			});

			WidgetToDisplay = ContainerWidget;
		}
	}
	
	if (UOverlaySlot* OSlot = Cast<UOverlaySlot>(Popups->AddChild(WidgetToDisplay)))
	{
		OSlot->SetHorizontalAlignment(HAlign_Center);
		OSlot->SetVerticalAlignment(VAlign_Center);

		DisplayedWidgets.Add(InUserWidget->GetId(), WidgetToDisplay);
	}
}

void UCavrnusDesktopCanvasWidgetDisplayer::DisplayDialogWidget(
	UCavrnusBaseDialogWidget* InUserWidget,
	const FCavrnusDialogOptions& Options,
	const TFunction<void(UCavrnusBaseDialogWidget*)>& OnWidgetClosed)
{
	ICavrnusWidgetDisplayer::DisplayDialogWidget(InUserWidget, Options, OnWidgetClosed);
}

void UCavrnusDesktopCanvasWidgetDisplayer::DisplayToastMessageWidget(
	UCavrnusBaseToastMessageWidget* InUserWidget,
	const FCavrnusToastMessageOptions& Options,
	const TFunction<void(UCavrnusBaseToastMessageWidget*)>& OnWidgetClosed)
{
	if (UVerticalBoxSlot* VSlot = Cast<UVerticalBoxSlot>(ToastMessages->AddChildToVerticalBox(InUserWidget)))
	{
		VSlot->SetHorizontalAlignment(HAlign_Center);
		VSlot->SetVerticalAlignment(VAlign_Center);

		DisplayedWidgets.Add(InUserWidget->GetId(), InUserWidget);
	}
}

void UCavrnusDesktopCanvasWidgetDisplayer::DisplayLoaderWidget(
	UCavrnusBaseUILoaderWidget* InWidgetToDisplay,
	const FCavrnusUILoaderOptions& Options,
	const TFunction<void(UCavrnusBaseUILoaderWidget*)>& OnWidgetClosed)
{
	if (UOverlaySlot* USlot = Cast<UOverlaySlot>(Loaders->AddChildToOverlay(InWidgetToDisplay)))
	{
		USlot->SetHorizontalAlignment(HAlign_Center);
		USlot->SetVerticalAlignment(VAlign_Center);

		DisplayedWidgets.Add(InWidgetToDisplay->GetId(), InWidgetToDisplay);
	}
}

void UCavrnusDesktopCanvasWidgetDisplayer::RemoveWidget(const FGuid& Id)
{
	if (const auto FoundWidgetPtr = DisplayedWidgets.Find(Id))
	{
		if (FoundWidgetPtr->IsValid())
			FoundWidgetPtr->Get()->RemoveFromParent();

		DisplayedWidgets.Remove(Id);
	}
}

void UCavrnusDesktopCanvasWidgetDisplayer::RemoveAll()
{
	for (TPair<FGuid, TWeakObjectPtr<UUserWidget>> DisplayedWidget : DisplayedWidgets)
	{
		if (DisplayedWidget.Value.IsValid())
			DisplayedWidget.Value->RemoveFromParent();
	}

	DisplayedWidgets.Empty();
}

void UCavrnusDesktopCanvasWidgetDisplayer::ShouldAddToViewport()
{
	if (!bIsInViewport)
	{
		AddToViewport();
		bIsInViewport = true;
	}
}