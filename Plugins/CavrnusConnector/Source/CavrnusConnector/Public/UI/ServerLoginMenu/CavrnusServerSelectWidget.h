// Copyright (c) 2024 Cavrnus. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CavrnusServerSelectWidget.generated.h"

class UEditableTextBox;
class UButton;
UCLASS()
class CAVRNUSCONNECTOR_API UCavrnusSeverSelectWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, Category="Cavrnus|ServerWidget", meta=(BindWidget))
	UEditableTextBox* ServerTextBox = nullptr;
	
	UPROPERTY(BlueprintReadWrite, Category="Cavrnus|ServerWidget", meta=(BindWidget))
	UButton* ProceedButton = nullptr;
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
private:
	UFUNCTION()
	void ProceedButtonClicked();
	UFUNCTION()
	void ServerTextBoxUpdated(const FText& Text);
};