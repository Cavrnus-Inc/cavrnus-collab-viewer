// // Copyright (c) 2025 Cavrnus. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CavrnusListContainerInterface.h"
#include "CavrnusListView.h"
#include "UI/CavrnusBaseUserWidget.h"
#include "UI/Components/Text/CavrnusUITextBlock.h"
#include "CavrnusListContainerWidget.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class CAVRNUSCONNECTOR_API UCavrnusListContainerWidget : public UCavrnusBaseUserWidget, public ICavrnusListContainerInterface
{
	GENERATED_BODY()
public:
	FCavrnusListItemSelected ItemSelectedCallback;
	FCavrnusListItemGenerated WidgetGeneratedCallback;

	UPROPERTY()
	UObject* LastSelectedItem = nullptr;
	
	void SetLoaderState(const bool InState);
	void SetLoaderLabel(const FText& InText);

	void SetFeedbackLabel(const FText& InText);

	// IGenericListContainerInterface
	virtual FCavrnusListItemSelected& OnItemSelected() override;
	virtual FCavrnusListItemGenerated& OnEntryGenerated() override;

	virtual void InitializeList(TSubclassOf<UCavrnusBaseListItemWidget> EntryWidgetClass) override;
	virtual void AddItemAt(UObject* Item, int32 Index) override;
	virtual void AddItem(UObject* Item) override;
	virtual void RemoveItem(UObject* Item) override;
	virtual void ClearItems() override;
	virtual void SetSelectedItem(UObject* Item) override;
	virtual void TeardownList() override;
	// end IGenericListContainerInterface

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Cavrnus|ListContainer")
	FMargin ItemPadding = FMargin();
	
	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UCavrnusListView> ListView = nullptr;

	// UPROPERTY(meta=(BindWidgetOptional), Category="Cavrnus|ListContainer")
	// TObjectPtr<UCavrnusTwinmotionUISpinnerWidget> SpinnerWidget = nullptr;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UCavrnusUITextBlock> FeedbackTextBlock = nullptr;
	
private:
	UPROPERTY()
	TArray<UUserWidget*> TrackedWidgetItems;
};
