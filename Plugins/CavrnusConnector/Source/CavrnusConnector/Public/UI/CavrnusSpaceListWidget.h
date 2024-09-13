// Copyright(c) Cavrnus. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include <Blueprint/UserWidget.h>

#include "CavrnusBaseUserWidget.h"
#include "Components/EditableTextBox.h"
#include "Pagination/Pagination.h"
#include "Types/CavrnusBinding.h"
#include "Types/CavrnusSpaceInfo.h"
#include "CavrnusSpaceListWidget.generated.h"

/**
 * @brief Delegate for handling the selection of a Cavrnus space.
 * @param FString The ID of the selected space.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCavrnusSpaceSelected, FCavrnusSpaceInfo, SpaceInfo);

/**
 * @brief Type definition for a function that handles space selection events.
 * @param FCavrnusSpaceInfo Information about the selected space.
 */
typedef TFunction<void(const FCavrnusSpaceInfo&)> FSpaceSelectedEvent;

/**
 * @brief Widget class for displaying and managing a list of Cavrnus spaces.
 * This widget allows users to search, paginate, and select spaces from a list.
 */
UCLASS(Abstract)
class CAVRNUSCONNECTOR_API UCavrnusSpaceListWidget : public UCavrnusBaseUserWidget
{
	GENERATED_BODY()

	/**
	 * @brief Helper class representing a space list option.
	 * Implements IListElementInterface to handle the UI representation of a space.
	 */
	class FSpaceListOption : public IListElementInterface
	{
	public:
		/**
		 * @brief Constructor for FSpaceListOption.
		 * @param SpaceInfo Information about the space.
		 * @param OnSelectSpace Function to call when the space is selected.
		 */
		FSpaceListOption(const FCavrnusSpaceInfo& SpaceInfo, const FSpaceSelectedEvent& OnSelectSpace);

		/**
		 * @brief Called when an entry is built in the list.
		 * @param Element The widget element representing the entry.
		 */
		virtual void EntryBuilt(UUserWidget* Element) override;

		/** Structure to hold information about a Cavrnus space. */
		FCavrnusSpaceInfo Content = FCavrnusSpaceInfo();
		
		/** Function to call when the space is selected. */
		FSpaceSelectedEvent OnSelect;
	};

protected:
	/** Construct the widget by calling chain of parent setup calls */
	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

public:
	FSpaceSelectedEvent SpaceSelected;
	
	/**
	 * @brief Searches the space list based on the input search value.
	 * @param SearchValue The text to search for.
	 */
	UFUNCTION()
	void Search(const FText& SearchValue);

	void FetchSpaces();

	/** The widget class for pagination items. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cavrnus|SpaceList")
	TSubclassOf<UUserWidget> PaginationItemWidget = nullptr;

	/** The pagination widget for navigating through spaces. */
	UPROPERTY(BlueprintReadOnly, Category = "Cavrnus|SpaceList", meta = (BindWidget))
	UPagination* SpacePagination = nullptr;

	/** The text box for entering search terms. */
	UPROPERTY(EditAnywhere, Category = "Cavrnus|SpaceList", meta = (BindWidget))
	UEditableTextBox* SearchTextBox = nullptr;

	UPROPERTY(EditAnywhere, Category = "Cavrnus|SpaceList", meta = (BindWidget))
	UButton* ButtonCreateSpace = nullptr;

	/** Delegate for handling the selection of a Cavrnus space. */
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Cavrnus|SpaceList")
	FOnCavrnusSpaceSelected OnCavrnusSpaceSelected;

private:
	UPROPERTY()
	UCavrnusBinding* JoinableSpacesBinding;
	
	/** List of all spaces available. */
	UPROPERTY()
	TArray<FCavrnusSpaceInfo> AllSpaces;
	
	/** List of currently displayed spaces based on search and pagination. */
	UPROPERTY()
	TArray<FCavrnusSpaceInfo> CurrentDisplayedSpaces;
	
	/**
	 * @brief Updates the pagination with the provided list of spaces.
	 * @param Spaces The list of spaces to display.
	 */
	void UpdatePagination(TArray<FCavrnusSpaceInfo>& Spaces);

	UFUNCTION()
	void BlueprintSelectedSpaceToJoin(FCavrnusSpaceInfo SpaceInfo);
};