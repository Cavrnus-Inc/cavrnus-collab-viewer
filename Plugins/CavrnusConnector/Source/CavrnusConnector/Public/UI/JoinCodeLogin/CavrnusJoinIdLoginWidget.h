// Copyright (c) 2024 Cavrnus. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include <Styling/SlateTypes.h>
#include "CavrnusJoinIdLoginWidget.generated.h"

class UTextBlock;
class UCavrnusInputField;
class UButton;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnJoinIdWidgetCloseButtonClicked);

UCLASS()
class CAVRNUSCONNECTOR_API UCavrnusJoinIdLoginWidget : public UUserWidget
{
	GENERATED_BODY()

	// JoinCode Fields
	UPROPERTY(meta=(BindWidget))
	UCavrnusInputField* JoinCodeInputField = nullptr;
	
	UPROPERTY(meta=(BindWidget))
	UButton* TryJoinSpaceButton = nullptr;

	UPROPERTY(meta=(BindWidget))
	UButton* CloseButton = nullptr;
	
	UPROPERTY(meta=(BindWidget))
	UTextBlock* JoinSpaceButtonText = nullptr;

	UPROPERTY(BlueprintAssignable, Category = "Cavrnus|InputField")
	FOnJoinIdWidgetCloseButtonClicked OnCloseButtonClicked;
	
	UFUNCTION()
	void JoinCodeInputUpdated(const FString& Text);
	UFUNCTION()
	void JoinCodeInputCommitted(const FText& Text, ETextCommit::Type CommitMethod);
	
	UFUNCTION()
	void TryJoinSpaceButtonClicked();

	UFUNCTION()
	void CloseButtonClicked();
	
	void SetJoinSpaceButtonText(const FString& Value);

	void SetCloseButtonVis(bool bVis);
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
private:
	UPROPERTY()
	UButton* CurrentSelectedViewButton;
	UPROPERTY()
	FButtonStyle InitialButtonStyle;
	UPROPERTY()
	FButtonStyle ActivatedStyle;
	
	void TryJoinSpace(const FString& JoinId);
};