// Copyright (c) 2025 Cavrnus. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CavrnusCvtSetupButton.h"
#include "Blueprint/UserWidget.h"
#include "Components/VerticalBox.h"
#include "CavrnusCVTEditorSetupWidget.generated.h"

UCLASS(Abstract)
class CAVRNUSCVTEDITOR_API UCavrnusCVTEditorSetupWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
private:
	enum EButtonTypeEnum
	{
		Required,
		Optional,
	};

	FDelegateHandle WorldDelegate = FDelegateHandle();
	
	UPROPERTY()
	TArray<UCavrnusCvtSetupButton*> Buttons;
	
	UPROPERTY(EditAnywhere, Category = "CavrnusCVT ")
	TSubclassOf<UCavrnusCvtSetupButton> SetupButtonBlueprint = nullptr;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UVerticalBox> ButtonContainerRequired = nullptr;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UVerticalBox> ButtonContainerOptional = nullptr;

	UCavrnusCvtSetupButton* CreateButton(const EButtonTypeEnum& ButtonType);
};
