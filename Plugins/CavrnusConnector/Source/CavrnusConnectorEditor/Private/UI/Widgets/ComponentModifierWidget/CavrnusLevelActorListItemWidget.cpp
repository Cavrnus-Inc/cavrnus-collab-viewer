// // Copyright (c) 2025 Cavrnus. All rights reserved.

#include "UI/Widgets/ComponentModifierWidget/CavrnusLevelActorListItemWidget.h"

UCavrnusLevelActorListItemWidget* UCavrnusLevelActorListItemWidget::Setup()
{
	return this;
}

UCavrnusLevelActorListItemWidget* UCavrnusLevelActorListItemWidget::BindSelected(const TFunction<void()>& OnSelected)
{
	OnSelectedCallback.AddLambda(OnSelected);
	return this;
}

void UCavrnusLevelActorListItemWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	Super::NativeOnListItemObjectSet(ListItemObject);

	// const UListObjectSpaceMember* FoundObj = Cast<UListObjectSpaceMember>(ListItemObject);
	// if (!IsValid(FoundObj))
	// 	return;
	//
	// SetupWidget(FoundObj->SpaceMember, FoundObj->SpaceInfo);
}

void UCavrnusLevelActorListItemWidget::NativeOnEntryReleased()
{
	Super::NativeOnEntryReleased();
}
