// Copyright (c) 2024 Cavrnus. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CavrnusBaseUserWidget.h"
#include "CavrnusGuestLoginWidget.generated.h"

class UEditableTextBox;
class UButton;
/**
 * @brief UCavrnusGuestLoginWidget class.
 *
 * This abstract class extends UUserWidget to provide functionality for guest login.
 */
UCLASS(Abstract)
class CAVRNUSCONNECTOR_API UCavrnusGuestLoginWidget : public UCavrnusBaseUserWidget
{
	GENERATED_BODY()
public:

	/**
	 * @brief Sets up the guest login widget.
	 *
	 * This function is responsible for initializing the widget and preparing it for user interaction.
	 */
	UFUNCTION(BlueprintCallable, Category = "Cavrnus|Login", meta = (BindWidget))
	void Setup();

	virtual void NativeDestruct() override;

	/**
	 * @brief Handles the login button click event.
	 *
	 * This function is called when the login button is clicked, initiating the guest login process.
	 */
	UFUNCTION()
	void OnLoginClicked();

	/**
	 * @brief Editable text box for guest username input.
	 *
	 * This property binds to a UI widget that allows the user to input their guest username.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Cavrnus|Login", meta = (BindWidget))
	UEditableTextBox* GuestUsernameInput = nullptr;

	/**
	 * @brief Button for initiating guest login.
	 *
	 * This property binds to a UI widget that serves as the login button.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Cavrnus|Login", meta = (BindWidget))
	UButton* LoginButton = nullptr;
};