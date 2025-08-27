// Copyright (c) 2025 Cavrnus. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Components/Buttons/CavrnusUIButton.h"
#include "UI/Components/Text/CavrnusUITextBlock.h"
#include "CavrnusCvtSetupButton.generated.h"

UCLASS(abstract)
class CAVRNUSCVTEDITOR_API UCavrnusCvtSetupButton : public UCavrnusUIButton
{
	GENERATED_BODY()
public:
	UCavrnusCvtSetupButton* SetPrimaryText(const FString& InPrimaryText);
	UCavrnusCvtSetupButton* SetSecondaryText(const FString& InPrimaryText);
	UCavrnusCvtSetupButton* SetCompletionState(const bool bIsComplete);
	UCavrnusCvtSetupButton* ShowCheckMarkContainer(const bool bShow);

protected:
	virtual void NativePreConstruct() override;

private:
	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UCavrnusUITextBlock> PrimaryText = nullptr;
	
	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UCavrnusUITextBlock> SecondaryText = nullptr;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UPanelWidget> Checkmark = nullptr;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UPanelWidget> CheckmarkContainer = nullptr;
};
