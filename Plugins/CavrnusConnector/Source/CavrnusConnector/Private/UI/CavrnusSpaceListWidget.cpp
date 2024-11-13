// Copyright(c) Cavrnus. All rights reserved.

#include "UI/CavrnusSpaceListWidget.h"

#include "CavrnusSpatialConnectorSubSystem.h"
#include "CavrnusSpatialConnector.h"
#include "CavrnusFunctionLibrary.h"
#include "Components/Button.h"
#include "UI/SpaceList/SpaceListOption.h"

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
			UCavrnusFunctionLibrary::GetCavrnusSpatialConnector()->SpaceId = SpaceInfo.SpaceId;
			UCavrnusFunctionLibrary::JoinSpace(SpaceInfo.SpaceId, [](const FCavrnusSpaceConnection&) {  }, [](const FString&) {});
			this->RemoveFromParent();
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
