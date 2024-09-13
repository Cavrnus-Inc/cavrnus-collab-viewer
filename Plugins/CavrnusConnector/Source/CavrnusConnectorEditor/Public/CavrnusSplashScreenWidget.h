// Copyright(c) Cavrnus. All rights reserved.
#pragma once
#include <EditorUtilityWidget.h>

#include "Components/EditableTextBox.h"
#include "CavrnusSplashScreenWidget.generated.h"

class UCheckBox;
class UButton;

UCLASS()
class UCavrnusSplashScreenWidget : public UEditorUtilityWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly, Category = "Cavrnus", meta = (BindWidget))
	UEditableTextBox* ServerInputField;

	UPROPERTY(BlueprintReadOnly, Category = "Cavrnus", meta = (BindWidget))
	UButton* SetupSpaceButton;

	UPROPERTY(BlueprintReadOnly, Category = "Cavrnus", meta = (BindWidget))
	UButton* OpenWebConsoleButton;

	UPROPERTY(BlueprintReadOnly, Category = "Cavrnus", meta = (BindWidget))
	UButton* OpenDocumentationButton;

	UPROPERTY(BlueprintReadOnly, Category = "Cavrnus", meta = (BindWidget))
	UButton* ButtonJoinDiscord;

	UPROPERTY(BlueprintReadOnly, Category = "Cavrnus", meta = (BindWidget))
	UCheckBox* ShowOnStartupCheckbox;

	UPROPERTY(BlueprintReadOnly, Category = "Cavrnus", meta = (BindWidget))
	UButton* DismissButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cavrnus")
	FString DiscordURL;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cavrnus")
	FString WebConsoleURL;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cavrnus")
	FString DocumentationURL;

	// Stored so can use to close the tab
	UPROPERTY()
	FName TabID;

	UFUNCTION()
	void OnSetupSpaceClicked();

	UFUNCTION()
	void OnWebConsoleClicked();

	UFUNCTION()
	void OnOpenDocsClicked();

	UFUNCTION()
	void OnJoinDiscordClicked();

	UFUNCTION()
	void OnShowSplashScreenOnStartupClicked(bool bCheckboxState);

	UFUNCTION()
	void OnDismissClicked();

	UFUNCTION()
	void OnServerInputFieldChanged(const FText& ServerVal);

private:
	FString CustomerServer;
	
	virtual void NativePreConstruct() override;

	void Close();
};