// // Copyright (c) 2025 Cavrnus. All rights reserved.

#include "UI/Components/Lists/CavrnusListContainerWidget.h"
#include "CavrnusConnectorModule.h"
#include "UI/Helpers/CavrnusUIScheduler.h"

void UCavrnusListContainerWidget::SetLoaderState(const bool InState)
{
	// if (SpinnerWidget)
	// 	SpinnerWidget->SetState(InState);
}

void UCavrnusListContainerWidget::SetLoaderLabel(const FText& InText)
{
	// if (SpinnerWidget)
	// 	SpinnerWidget->SetLabel(InText);
}

void UCavrnusListContainerWidget::SetFeedbackLabel(const FText& InText)
{
	if (FeedbackTextBlock)
		FeedbackTextBlock->SetText(InText);
}

FCavrnusListItemSelected& UCavrnusListContainerWidget::OnItemSelected()
{
	return ItemSelectedCallback;
}

FCavrnusListItemGenerated& UCavrnusListContainerWidget::OnEntryGenerated()
{
	return WidgetGeneratedCallback;
}

void UCavrnusListContainerWidget::InitializeList(const TSubclassOf<UCavrnusBaseListItemWidget> EntryWidgetClass)
{
	if (ListView == nullptr)
	{
		UE_LOG(LogCavrnusConnector, Error, TEXT("ListContainer is not valid! Cannot add item."));
		return;
	}

	ListView->OnItemClicked().Clear();
	ListView->OnEntryWidgetGenerated().Clear();
	ListView->OnItemClicked().Clear();
	
	if (ListView)
	{
		ListView->OnItemClicked().AddWeakLambda(this,[this] (UObject* ClickedObject)
		{
			if (ItemSelectedCallback.IsBound())
				ItemSelectedCallback.Broadcast(ClickedObject);
		});
	}

	ListView->OnEntryWidgetGenerated().AddLambda([this](UUserWidget& Widget)
   {
		// Cast to IUserObjectListEntry to get the underlying UObject
		if (IUserObjectListEntry* Entry = Cast<IUserObjectListEntry>(&Widget))
		{
			UObject* ItemObj = Entry->GetListItem<>();
			WidgetGeneratedCallback.Broadcast(&Widget, ItemObj);
		}
	   TrackedWidgetItems.Add(&Widget);
   });

	// SelectionMode needs to be set in order for SelectionChanged Delegate to work
	ListView->SetSelectionMode(ESelectionMode::Single);
	ListView->SetEntryWidgetClass(EntryWidgetClass);

	// Handle selection change, mainly for visual states
	ListView->OnItemSelectionChanged().AddWeakLambda(this, [this] (UObject* SelectedObj)
	{
		SetSelectedItem(SelectedObj);
	});
}

void UCavrnusListContainerWidget::AddItemAt(UObject* Item, const int32 Index)
{
	if (ListView == nullptr)
	{
		UE_LOG(LogCavrnusConnector, Error, TEXT("ListContainer is noCavrnusConnectoralid! Cannot add item."));
		return;
	}

	ListView->AddItemAt(Item, Index);
}

void UCavrnusListContainerWidget::AddItem(UObject* Item)
{
	if (ListView == nullptr)
	{
		UE_LOG(LogCavrnusConnector, Error, TEXT("ListContainer is noCavrnusConnectoralid! Cannot add item."));
		return;
	}

	if (!Item)
	{
		UE_LOG(LogCavrnusConnector, Warning, TEXT("Attempted to add CavrnusConnectorull widget to the list."));
		return;
	}
	
	ListView->AddItem(Item);
}

void UCavrnusListContainerWidget::RemoveItem(UObject* Item)
{
	if (ListView == nullptr)
	{
		UE_LOG(LogCavrnusConnector, Error, TEXT("ListContainer is noCavrnusConnectoralid! Cannot RemoveItem."));
		return;
	}

	if (!Item)
	{
		UE_LOG(LogCavrnusConnector, Warning, TEXT("Attempted to add CavrnusConnectorull widget to the list."));
		return;
	}
	
	ListView->RemoveItem(Item);
}

void UCavrnusListContainerWidget::ClearItems()
{
	if (ListView)
		ListView->ClearListItems();

	TrackedWidgetItems.Empty();
}

void UCavrnusListContainerWidget::SetSelectedItem(UObject* Item)
{
	// Waiting 2 frames...because well, if this is called when setting up the list, the listView hasn't created any objects yet
	FCavrnusUIScheduler::WaitOneFrame(GetWorld(),[this, Item]
	{
		FCavrnusUIScheduler::WaitOneFrame(GetWorld(), [this, Item]
		{
			if (ListView)
			{
				// Deselect previous
				if (LastSelectedItem && LastSelectedItem != Item)
				{
					if (UCavrnusBaseListItemWidget* PrevWidget = Cast<UCavrnusBaseListItemWidget>(ListView->GetEntryWidgetFromItem(LastSelectedItem)))
						PrevWidget->SetSelectedState(false);
				}

				// Select current
				if (UCavrnusBaseListItemWidget* CurrentWidget = Cast<UCavrnusBaseListItemWidget>(ListView->GetEntryWidgetFromItem(Item)))
					CurrentWidget->SetSelectedState(true);

				LastSelectedItem = Item;
			}
		});
	});
}

void UCavrnusListContainerWidget::TeardownList()
{
}
