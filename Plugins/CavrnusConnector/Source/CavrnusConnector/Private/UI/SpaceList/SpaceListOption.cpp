// Copyright(c) Cavrnus. All rights reserved.
#include "UI/SpaceList/SpaceListOption.h"
#include "Blueprint/AsyncTaskDownloadImage.h"
#include "Components/Button.h"

class IHttpRequest;

void USpaceListOption::Setup(const FCavrnusSpaceInfo& InSpaceInfo, const FSpaceSelectedEvent& InOnSelect)
{
	SpaceInfo = InSpaceInfo;
	OnSelect = InOnSelect;
	
	if (Button)
		Button->OnClicked.AddDynamic(this, &USpaceListOption::ButtonSpaceSelected);

	if (SpaceNameTextBlock)
		SpaceNameTextBlock->SetText(FText::FromString(SpaceInfo.SpaceName));

	Thumbnail->SetVisibility(ESlateVisibility::Hidden);
	ThumbnailDefault->SetVisibility(ESlateVisibility::Visible);

	if (!SpaceInfo.SpaceThumbnail.IsEmpty())
	{
		DownloadTask = NewObject<UAsyncTaskDownloadImage>();
		DownloadTask->OnSuccess.AddDynamic(this, &USpaceListOption::OnGetThumbnail);
		DownloadTask->Start(SpaceInfo.SpaceThumbnail);
	}
}

void USpaceListOption::NativeDestruct()
{
	Super::NativeDestruct();

	if (Button)
		Button->OnClicked.RemoveDynamic(this, &USpaceListOption::ButtonSpaceSelected);
}

void USpaceListOption::OnGetThumbnail(UTexture2DDynamic* Texture)
{
	if (Texture)
	{
		Thumbnail->SetBrushFromTextureDynamic(Texture, true);
		Thumbnail->SetVisibility(ESlateVisibility::Visible);
		ThumbnailDefault->SetVisibility(ESlateVisibility::Hidden);
	}
}

void USpaceListOption::ButtonSpaceSelected()
{
	if (OnSelect)
		OnSelect(SpaceInfo);
}