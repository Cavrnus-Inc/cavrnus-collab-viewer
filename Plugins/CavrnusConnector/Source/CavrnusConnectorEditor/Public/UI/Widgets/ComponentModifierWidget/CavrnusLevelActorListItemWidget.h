// // Copyright (c) 2025 Cavrnus. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Components/Lists/CavrnusBaseListItemWidget.h"
#include "UI/Components/Text/CavrnusUITextBlock.h"
#include "CavrnusLevelActorListItemWidget.generated.h"

UCLASS(Abstract)
class CAVRNUSCONNECTOREDITOR_API UCavrnusLevelActorListItemWidget : public UCavrnusBaseListItemWidget
{
	GENERATED_BODY()
public:
	UCavrnusLevelActorListItemWidget* Setup();
	UCavrnusLevelActorListItemWidget* BindSelected(const TFunction<void()>& OnSelected);
	
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	virtual void NativeOnEntryReleased() override;
	
private:
	TMulticastDelegate<void()> OnSelectedCallback;
	
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UCavrnusUITextBlock> ActorNameTextBlock = nullptr;
};
