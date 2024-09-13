// Copyright(c) Cavrnus. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "IListElementInterface.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Pagination.generated.h"

/**
 * @brief Class representing a pagination system for a list of items in the UI.
 *
 * The UPagination class provides functionality for paginating a list of items, allowing users to navigate through pages
 * of content using "Next" and "Previous" buttons. It manages the display of items per page and handles the logic for
 * loading different pages.
 */

class UButton;

UCLASS(Abstract)
class UPagination : public UUserWidget
{
	GENERATED_BODY()
	
public:
	/**
	 * @brief Sets up the pagination widget with the given item widget class.
	 *
	 * @param InItemWidget The class of the item widget to use for pagination.
	 */
	void Setup(TSubclassOf<UUserWidget> InItemWidget);

	virtual void NativeDestruct() override;

	/** Button for navigating to the previous page. */
	UPROPERTY(BlueprintReadOnly, Category = "Cavrnus|Pagination", meta = (BindWidget))
	UButton* ButtonPrevious = nullptr;
	
	/** Button for navigating to the next page. */
	UPROPERTY(BlueprintReadOnly, Category = "Cavrnus|PaginationSetup", meta = (BindWidget))
	UButton* ButtonNext = nullptr;

	/** Text block displaying the current page number. */
	UPROPERTY(BlueprintReadOnly, Category = "Cavrnus|Pagination", meta = (BindWidget))
	UTextBlock* TextBlockCurrentPage = nullptr;

	/** Widget container to display when there are no results. */
	UPROPERTY(EditAnywhere, Category = "Cavrnus|Pagination")
	UWidget* NoResultsContainer = nullptr;

	/** Widget container for displaying paginated items. */
	UPROPERTY(EditAnywhere, Category = "Cavrnus|Pagination")
	UWidget* ItemContainer = nullptr;

	/** Footer container widget. */
	UPROPERTY(EditAnywhere, Category = "Cavrnus|Pagination", meta = (BindWidget))
	UPanelWidget* FooterContainer = nullptr;

	/**
	 * @brief Initializes a new pagination with the specified item widget class and content to display.
	 *
	 * @param InItemWidget The class of the item widget to use for pagination.
	 * @param InDisplayContent The content to display in the pagination.
	 */
	void NewPagination(TSubclassOf<UUserWidget> InItemWidget, const TArray<IListElementInterface*>& InDisplayContent);
	
	/** Resets the pagination, clearing any current items and content. */
	void ResetPagination();

	/**
	 * @brief Loads the specified page of content.
	 *
	 * @param Page The page number to load.
	 */
	void LoadPage(int Page);

	/** Handler for the "Next" button click event. */
	UFUNCTION()
	void Next();

	/** Handler for the "Previous" button click event. */
	UFUNCTION()
	void Previous();

	/** Number of items to display per page. */
	UPROPERTY(EditAnywhere, Category = "PaginationSetup")
	int ItemsPerPage = 2;

private:
	/** Map of current items being displayed, mapping widgets to their corresponding content interfaces. */
	TMap<UUserWidget*, IListElementInterface*> CurrentItems;

	/** Class of the item widget used for pagination. */
	TSubclassOf<UUserWidget> ItemWidget = nullptr;
	
	/** Array of content to paginate and display. */
	TArray<IListElementInterface*> Content;
	
	/** The current and total page numbers to display, based on the content and items per page. */
	int CurrentPage = 0;
	int TotalPages = 0;
};