// Copyright(c) Cavrnus. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/AsyncTaskDownloadImage.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "UI/CavrnusSpaceListWidget.h"
#include "SpaceListOption.generated.h"

/**
 * @brief A widget class for displaying space options in a list.
 *
 * This class represents an individual entry in a space list, showing details
 * like the space thumbnail and name. It allows users to select a space
 * by clicking a button.
 */

UCLASS(Abstract)
class CAVRNUSCONNECTOR_API USpaceListOption : public UUserWidget
{
	GENERATED_BODY()
	
public:
	/**
	 * @brief Sets up the space list option widget.
	 *
	 * @param InSpaceInfo Information about the space to be displayed.
	 * @param InOnSelect The event to be triggered when the space is selected.
	 */
	void Setup(const FCavrnusSpaceInfo& InSpaceInfo, const FSpaceSelectedEvent& InOnSelect);

	virtual void NativeDestruct() override;
	
	/**
	 * @brief Callback function for when the thumbnail image is downloaded.
	 *
	 * @param Texture The downloaded texture.
	 */
	UFUNCTION()
	void OnGetThumbnail(UTexture2DDynamic* Texture);

	/** The default thumbnail image displayed before the actual thumbnail is downloaded. */
	UPROPERTY(BlueprintReadOnly, Category = "Cavrnus", meta = (BindWidget))
	UImage* ThumbnailDefault = nullptr;
	
	/** The actual thumbnail image of the space. */
	UPROPERTY(BlueprintReadOnly, Category = "Cavrnus", meta = (BindWidget))
	UImage* Thumbnail = nullptr;
	
	/** The text block displaying the name of the space. */
	UPROPERTY(BlueprintReadOnly, Category = "Cavrnus", meta = (BindWidget))
	UTextBlock* SpaceNameTextBlock = nullptr;
	
	/** The button used to select the space. */
	UPROPERTY(BlueprintReadOnly, Category = "Cavrnus", meta = (BindWidget))
	UButton* Button = nullptr;

	/** The async task responsible for downloading the thumbnail image. */
	UPROPERTY()
	UAsyncTaskDownloadImage* DownloadTask = nullptr;

	/**
	 * @brief Callback function for when the space selection button is clicked.
	 */
	UFUNCTION()
	void ButtonSpaceSelected();

private:
	/** Information about the space. */
	UPROPERTY()
	FCavrnusSpaceInfo SpaceInfo = FCavrnusSpaceInfo();
	
	/** The event to be triggered when the space is selected. */
	FSpaceSelectedEvent OnSelect;
};