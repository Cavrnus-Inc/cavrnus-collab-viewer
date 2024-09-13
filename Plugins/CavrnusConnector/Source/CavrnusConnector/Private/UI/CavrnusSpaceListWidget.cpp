// Copyright(c) Cavrnus. All rights reserved.
#include "UI/CavrnusSpaceListWidget.h"

#include "CavrnusSpatialConnectorSubSystem.h"
#include "CavrnusFunctionLibrary.h"
#include "Components/Button.h"
#include "UI/SpaceList/SpaceListOption.h"

void UCavrnusSpaceListWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	SearchTextBox->OnTextChanged.AddDynamic(this, &UCavrnusSpaceListWidget::Search);
	SearchTextBox->SetIsEnabled(false);
	
	SpacePagination->Setup(PaginationItemWidget);

	OnCavrnusSpaceSelected.AddDynamic(this, &UCavrnusSpaceListWidget::BlueprintSelectedSpaceToJoin);

	FetchSpaces();
}

void UCavrnusSpaceListWidget::NativeDestruct()
{
	Super::NativeDestruct();

	SearchTextBox->OnTextChanged.RemoveDynamic(this, &UCavrnusSpaceListWidget::Search);
	OnCavrnusSpaceSelected.RemoveDynamic(this, &UCavrnusSpaceListWidget::BlueprintSelectedSpaceToJoin);
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
		const FSpaceSelectedEvent Callback = [this](const FCavrnusSpaceInfo& space)
		{
			if (SpaceSelected)
				SpaceSelected(space);
		};

		Options.Add(new FSpaceListOption(Space, Callback));
	}
	
	SpacePagination->NewPagination(PaginationItemWidget, Options);
}

void UCavrnusSpaceListWidget::BlueprintSelectedSpaceToJoin(FCavrnusSpaceInfo SpaceInfo)
{
	if (SpaceSelected)
		SpaceSelected(SpaceInfo);
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