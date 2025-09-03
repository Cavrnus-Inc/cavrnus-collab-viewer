// // Copyright (c) 2025 Cavrnus. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CavrnusListContainerInterface.h"
#include "CavrnusListViewDataObject.h"

template <typename DataType>
class TCavrnusUIListHandler
{
public:
	static TUniquePtr<TCavrnusUIListHandler> QuickInit(
		ICavrnusListContainerInterface* InContainer,
		const TSubclassOf<UCavrnusBaseListItemWidget>& InWidgetBlueprint,
		const TFunction<FString(const DataType& Data)> InKeyFunc,
		const TFunction<bool(const DataType& A, const DataType& B)> InSortPredicate)
	{
		auto Handler = MakeUnique<TCavrnusUIListHandler>();
		Handler->Initialize(InContainer, InWidgetBlueprint, InKeyFunc, InSortPredicate);
		return Handler;
	}
	
	void Initialize(
		ICavrnusListContainerInterface* InContainer,
		const TSubclassOf<UCavrnusBaseListItemWidget>& InWidgetBlueprint,
		const TFunction<FString(const DataType& Data)> InKeyFunc,
		const TFunction<bool(const DataType& A, const DataType& B)> InSortPredicate)
	{
		Container = InContainer;
		UserWidgetBlueprint = InWidgetBlueprint;
		KeyFunc = InKeyFunc;
		SortPredicate = InSortPredicate;

		if (Container)
			Container->InitializeList(InWidgetBlueprint);
	}

	void AddItems(const TArray<DataType> Items)
	{
		for (const auto Item : Items)
			AddItem(Item);
	}

	void AddItem(const DataType& Item)
	{
		// this is a bit of a bottleneck...
		DataArray.Add(Item);
		// UE_LOG(LogTemp, Error, TEXT("[AddItem] Count: %d"), SortedDataArray.Num());
	
		auto Index = DataArray.Num() - 1;

		// If we provided a sorter, then sort and grab the resulting sorted index
		if (SortPredicate)
		{
			DataArray.Sort(SortPredicate);
			// Lets grab the index so we can insert to the proper position
			Index = FindElementIndex(Item);
			if (Index == INDEX_NONE)
			{
				UE_LOG(LogTemp, Error, TEXT("INDEX_NONE! Cannot AddItem!"));
				return;
			}
		}

		const FString Key = KeyFunc(Item);
		
		auto DataObj = NewObject<UCavrnusListViewDataObject>();
		DataObj->Id = Key;

		DataMap.Add(Key, Item);
		ObjectMap.Add(Key, DataObj);
		Container->AddItemAt(DataObj, Index);
	}

	void UpdateItem(const DataType& Item)
	{
		const FString Key = KeyFunc(Item);

		// Update data first
		int32 OldIndex = FindElementIndex(Item);
		if (OldIndex == INDEX_NONE)
		{
			UE_LOG(LogTemp, Error, TEXT("Updating Item! Unable to find, so Index == INDEX_NONE!"));
			return;
		}
		
		DataArray[OldIndex] = Item;

		// Sort now
		if (SortPredicate)
			DataArray.Sort(SortPredicate);

		UE_LOG(LogTemp, Error, TEXT("[UpdateItem] Count: %d"), DataArray.Num());

		const int32 NewIndex = FindElementIndex(Item);
		if (NewIndex == INDEX_NONE)
		{
			UE_LOG(LogTemp, Error, TEXT("After sort, item still not found — this should not happen."));
			return;
		}

		// Remove old object AFTER list state is settled
		if (UObject** OldDataObjectPtr = ObjectMap.Find(Key))
		{
			UObject* OldDataObject = *OldDataObjectPtr;
			Container->RemoveItem(OldDataObject);
			ObjectMap.Remove(Key);

			UE_LOG(LogTemp, Error, TEXT("[Update] Removed old data object"));
		}

		// Create and reinsert
		UObject* NewDataObj = NewObject<UCavrnusListViewDataObject>();
		ObjectMap.Add(Key, NewDataObj);
		Container->AddItemAt(NewDataObj, NewIndex);
	}

	void RemoveItem(const DataType& Item)
	{
		const FString Key = KeyFunc(Item);
		if (UObject** FoundDataObjectPtr = ObjectMap.Find(Key))
		{
			Container->RemoveItem(*FoundDataObjectPtr);
			ObjectMap.Remove(Key);
			DataMap.Remove(Key);
			DataArray.Remove(Item);
		
			UE_LOG(LogTemp, Error, TEXT("[RemoveItem] Removing Item! AFTER Count: %d"), DataArray.Num());
		}
	}

	void OnItemSelected(const TFunction<void(const DataType& SelectedData)>& OnSelected)
	{
		UE_LOG(LogTemp, Error, TEXT("[OnItemSelected] Attempting to select item..."));
		
		if (!Container || !OnSelected)
			return;

		for (auto Handle : SelectedHandles)
			Container->OnItemSelected().Remove(Handle);
		
		SelectedHandles.Empty();

		auto Handle = Container->OnItemSelected().AddLambda([this, OnSelected] (const UObject* SelectedObj)
		{
			if (!IsValid(SelectedObj))
			{
				UE_LOG(LogTemp, Error, TEXT("[OnItemSelected] This is a borked SelectedObj..."));
				return;
			}

			if (auto CastedObj = Cast<UCavrnusListViewDataObject>(SelectedObj))
			{
				if (auto FoundData = DataMap.Find(CastedObj->Id))
					OnSelected(*FoundData);
				else
					UE_LOG(LogTemp, Error, TEXT("[OnItemSelected] Unable to find requested data!!..."));
			} else
			{
				UE_LOG(LogTemp, Error, TEXT("[OnItemSelected] Unable to cast!!..."));
			}
		});

		SelectedHandles.Add(Handle);
	}

	template<typename WidgetType>
	void OnWidgetCreated(const TFunction<void(WidgetType* Widget, DataType Data)>& OnCreatedWidget)
	{
		if (Container)
		{
			auto Handle = Container->OnEntryGenerated().AddLambda([OnCreatedWidget, this](UUserWidget* UserWidget, UObject* Object)
			{
				if (!IsValid(UserWidget))
					return;

				auto CastedWidget = Cast<WidgetType>(UserWidget);
				if (CastedWidget == nullptr)
				{
					UE_LOG(LogTemp, Error, TEXT("[OnWidgetCreated] CastedWidget failed!"));
					return;
				}

				if (UCavrnusListViewDataObject* CastedData = Cast<UCavrnusListViewDataObject>(Object))
				{
					if (DataType* FoundData = DataMap.Find(CastedData->Id))
					{
						if (OnCreatedWidget)
							OnCreatedWidget(CastedWidget, *FoundData);	
					}
				}
			});

			WidgetCreatedHandles.Add(Handle);
		}
	}
	
	void ClearItems()
	{
		for (const auto Item : ObjectMap)
			Container->RemoveItem(Item.Value);

		ObjectMap.Empty();
		DataMap.Empty();
		DataArray.Empty();
	}

	void Teardown()
	{
		if (Container)
		{
			Container->OnEntryGenerated().Clear();
			Container->OnItemSelected().Clear();
		}

		for (auto Handle : SelectedHandles)
		{
			if (Container)
				Container->OnItemSelected().Remove(Handle);
			Handle.Reset();
		}

		for (auto Handle : WidgetCreatedHandles)
		{
			if (Container)
				Container->OnEntryGenerated().Remove(Handle);
			Handle.Reset();
		}

		ClearItems();

		KeyFunc = nullptr;
		Container = nullptr;
		SortPredicate = nullptr;
		OnWidgetCreatedCallback = nullptr;
	}

	void SetSelectedItem(DataType Item);

private:
	TArray<FDelegateHandle> SelectedHandles;
	TArray<FDelegateHandle> WidgetCreatedHandles;
	
	TArray<DataType> DataArray;
	TMap<FString, DataType> DataMap;
	TMap<FString, UObject*> ObjectMap;
	
	// Widget data
	UPROPERTY()
	ICavrnusListContainerInterface* Container = nullptr;
	TSubclassOf<UCavrnusBaseListItemWidget> UserWidgetBlueprint;

	// Sort
	TFunction<FString(const DataType&)> KeyFunc;
	TFunction<bool(const DataType&, const DataType&)> SortPredicate;

	// Callbacks
	TFunction<void(DataType)> OnCreate;
	TFunction<void(UUserWidget*)> OnWidgetCreatedCallback;
	
	bool Equals(DataType A, DataType B)
	{
		return KeyFunc(A) == KeyFunc(B);
	}

	int32 FindElementIndex(const DataType& Item)
	{
		const FString Key = KeyFunc(Item);
		const int32 Index = DataArray.IndexOfByPredicate([&](const DataType& Elem) {
			 return KeyFunc(Elem) == Key;
		 });

		return Index;
	}
};


template <typename DataType>
void TCavrnusUIListHandler<DataType>::SetSelectedItem(DataType Item)
{
	const FString Key = KeyFunc(Item);

	if (UObject** FoundObjPtr = ObjectMap.Find(Key))
	{
		if (IsValid(*FoundObjPtr))
		{
			Container->SetSelectedItem(*FoundObjPtr);
		}
	}
	else
		UE_LOG(LogTemp, Error, TEXT("[SetSelectedItem] Can't find item!"));
}
