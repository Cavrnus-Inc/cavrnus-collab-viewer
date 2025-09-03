// // Copyright (c) 2025 Cavrnus. All rights reserved.

#include "UI/Components/Lists/CavrnusBaseListItemWidget.h"
#include "Components/ListView.h"

void UCavrnusBaseListItemWidget::SetListIndex(int32 InIndex)
{
}

void UCavrnusBaseListItemWidget::SetSelectedState(const bool bState)
{
}

void UCavrnusBaseListItemWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	if (const UListView* OwningListView = Cast<UListView>(GetOwningListView()))
	{
		const int32 Index = OwningListView->GetListItems().Find(ListItemObject);
		SetListIndex(Index);
	}
}

void UCavrnusBaseListItemWidget::ListViewSetSelectedItem()
{
	// No idea of how else to do this...why isn't SetSelectedItem in the base list view class?
	if (UListView* ListView = Cast<UListView>(GetOwningListView()))
		ListView->SetSelectedItem(GetListItem());
}