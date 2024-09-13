// Copyright(c) Cavrnus. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "CavrnusBaseUserWidget.h"
#include "CavrnusGuestLoginWidget.generated.h"

/**
 * @brief Base class for widget for obtaining guest login username.
 *
 * This class provides the user interface for entering a guest username and initiating the guest login process.
 */

DECLARE_MULTICAST_DELEGATE_OneParam(FOnGuestLoginClicked, FString);

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
	class UEditableTextBox* GuestUsernameInput = nullptr;;

	/**
	 * @brief Button for initiating guest login.
	 *
	 * This property binds to a UI widget that serves as the login button.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Cavrnus|Login", meta = (BindWidget))
	class UButton* LoginButton = nullptr;;

	/**
	 * @brief Delegate for handling guest login click events.
	 *
	 * This delegate is triggered when the login button is clicked, passing the guest username as a parameter.
	 */
	FOnGuestLoginClicked OnLogin;
};