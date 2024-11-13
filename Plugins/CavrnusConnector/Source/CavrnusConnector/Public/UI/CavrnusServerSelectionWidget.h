// Copyright(c) Cavrnus. All rights reserved.

#pragma once


#include "CoreMinimal.h"

#include "CavrnusBaseUserWidget.h"

#include "CavrnusServerSelectionWidget.generated.h"
class UTexture2D;

/**
 * @brief Base class for widget for obtaining login credentials.
 *
 * This widget provides a user interface for inputting email and password credentials,
 * and handles the login process within the Cavrnus platform.
 */

DECLARE_MULTICAST_DELEGATE_OneParam(FOnProceedClicked, FString);

UCLASS(Abstract)
class CAVRNUSCONNECTOR_API UCavrnusServerSelectionWidget : public UCavrnusBaseUserWidget
{
	GENERATED_BODY()

public:
	/**
	 * @brief Sets up the login widget.
	 *
	 * This function initializes the widget and prepares it for user interaction.
	 */
	UFUNCTION(BlueprintCallable, Category = "Cavrnus|Server")
	void Setup();

	virtual void NativeDestruct() override;
	
	/**
	 * @brief Handles the login button click event.
	 *
	 * This function is called when the login button is clicked, triggering the login process.
	 */
	UFUNCTION()
	void OnProceedClicked();

	/** The text box for entering the email address. */
	UPROPERTY(BlueprintReadOnly, Category = "Cavrnus|Server", meta = (BindWidget))
	class UEditableTextBox* DomainInput = nullptr;

	/** The button for initiating the login process. */
	UPROPERTY(BlueprintReadOnly, Category = "Cavrnus|Server", meta = (BindWidget))
	class UButton* ProceedButton = nullptr;

	/** Delegate for handling the login click event. */
	FOnProceedClicked OnProceed;
};