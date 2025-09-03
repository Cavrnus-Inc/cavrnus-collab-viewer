// Copyright (c) 2025 Cavrnus. All rights reserved.

#include "UI/Menus/SpaceList/CavrnusSpaceListWidget.h"


#include "CavrnusFunctionLibrary.h"
#include "UI/CavrnusUI.h"
#include "UI/Menus/SpaceList/SpaceListOption.h"
#include "UI/Systems/Messages/CavrnusScopedMessages.h"
#include "UI/Systems/Messages/ToastMessages/CavrnusToastMessageUISystem.h"
#include "UI/Systems/Messages/ToastMessages/Info/CavrnusInfoToastMessageWidget.h"
#include "UI/Systems/Pagination/Pagination.h"
#include "UI/Systems/RawWidgetHost/CavrnusRawWidgetHost.h"

void UCavrnusSpaceListWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	SearchTextBox->OnTextChanged.AddDynamic(this, &UCavrnusSpaceListWidget::Search);
	SearchTextBox->SetIsEnabled(false);
	
	SpacePagination->Setup(PaginationItemWidget);

	FetchSpaces();
}

void UCavrnusSpaceListWidget::NativeDestruct()
{
	Super::NativeDestruct();

	SearchTextBox->OnTextChanged.RemoveDynamic(this, &UCavrnusSpaceListWidget::Search);

	// this assumes we are only using this popup in the generic UI flow manager...
	UCavrnusUI::Get(this)->GenericWidgetDisplayer()->Close(this);
}

void UCavrnusSpaceListWidget::Search(const FText& SearchValue)
{
	CurrentDisplayedSpaces.Empty();

	if (SearchValue.IsEmpty())
	{
		SpacePagination->ResetPagination();
		UpdatePagination(AllSpaces);
		
		return;
	}

	for (auto Space : AllSpaces)
	{
		if (Space.SpaceName.ToLower().Contains(SearchValue.ToString().ToLower()))
			CurrentDisplayedSpaces.Add(Space);
	}

	UpdatePagination(CurrentDisplayedSpaces);
}

void UCavrnusSpaceListWidget::FetchSpaces()
{
	UCavrnusFunctionLibrary::FetchJoinableSpaces([this](const TArray<FCavrnusSpaceInfo>& Spaces)
	{
		AllSpaces = Spaces;
		CurrentDisplayedSpaces = Spaces;

		UpdatePagination(AllSpaces);

		SearchTextBox->SetIsEnabled(true);
	});
}

void UCavrnusSpaceListWidget::UpdatePagination(TArray<FCavrnusSpaceInfo>& Spaces)
{
	Spaces.Sort([](const FCavrnusSpaceInfo& A, const FCavrnusSpaceInfo& B)
	{
		return A.LastAccess > B.LastAccess;
	});

	TArray<IListElementInterface*> Options;

	for (const FCavrnusSpaceInfo& Space : Spaces)
	{
		const FSpaceSelectedEvent SpaceSelectedEvent = [this](const FCavrnusSpaceInfo& SpaceInfo)
		{
			UCavrnusFunctionLibrary::JoinSpace(SpaceInfo.SpaceId, [this](const FCavrnusSpaceConnection& Sc)
			{

			}, [](const FString& Err)
			{
				UCavrnusUI::Get()->Messages()->Toast()->CreateAutoClose<UCavrnusInfoToastMessageWidget>()
					->SetPrimaryText("Failed to Join Space")
					->SetSecondaryText(FString::Printf(TEXT("%s"), *Err))
					->SetType(ECavrnusInfoToastMessageEnum::Warning);
			});

			// this assumes we are only using this popup in the generic UI flow manager...
			UCavrnusUI::Get(this)->GenericWidgetDisplayer()->Close(this);
		};

		Options.Add(new FSpaceListOption(Space, SpaceSelectedEvent));
	}
	
	SpacePagination->NewPagination(PaginationItemWidget, Options);
}

UCavrnusSpaceListWidget::FSpaceListOption::FSpaceListOption(const FCavrnusSpaceInfo& SpaceInfo, const FSpaceSelectedEvent& OnSelectSpace)
{
	Content = SpaceInfo;
	OnSelect = OnSelectSpace;
}

void UCavrnusSpaceListWidget::FSpaceListOption::EntryBuilt(UUserWidget* Element)
{
	if (Element == nullptr)
		return;

	if (USpaceListOption* ListOption = Cast<USpaceListOption>(Element))
		ListOption->Setup(Content, OnSelect);
}
