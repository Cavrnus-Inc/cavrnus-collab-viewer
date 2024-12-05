// Copyright (c) 2024 Cavrnus. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CavrnusInputField.generated.h"

class UBorder;
class UTextBlock;
class UEditableTextBox;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInputUpdated, const FString&, InputText);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInputCommitted, const FString&, InputText, ETextCommit::Type, CommitMethod);

UCLASS()
class CAVRNUSCONNECTOR_API UCavrnusInputField : public UUserWidget
{
	GENERATED_BODY()

public:
	enum EInputFieldState
	{
		Normal,
		Valid,
		Invalid,
		Disabled
	};

	EInputFieldState CurrentState;
	
	UPROPERTY(BlueprintReadWrite, Category = "Cavrnus|InputField", EditAnywhere, meta=(BindWidget))
	UEditableTextBox* InputField = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "Cavrnus|InputField", EditAnywhere, meta=(BindWidget))
	UBorder* InputFieldBorder = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "Cavrnus|InputField", meta=(BindWidget))
	UTextBlock* MessageText = nullptr;

	UPROPERTY(BlueprintAssignable, Category = "Cavrnus|InputField")
	FOnInputUpdated OnInputUpdated;

	UPROPERTY(BlueprintAssignable, Category = "Cavrnus|InputField")
	FOnInputCommitted OnInputCommitted;

	UPROPERTY(EditAnywhere, Category="Cavrnus|InputField", meta=(BindWidget))
	FLinearColor ValidColor;

	UPROPERTY(EditAnywhere, Category="Cavrnus|InputField", meta=(BindWidget))
	FLinearColor InvalidColor;

	// Configuration
	UPROPERTY(EditAnywhere, Category="Cavrnus|InputField", meta=(BindWidget))
	bool IsPasswordField = false;

	UFUNCTION()
	void FieldUpdated(const FText& Text);
	UFUNCTION()
	void FieldCommitted(const FText& Text, ETextCommit::Type CommitMethod);

	bool HasAnyText();
	void ClearText();
	FString GetInputString();

	void SetState(EInputFieldState NewState);
	void SetMessage(const FString& Message);
	void SetFieldColor(const FLinearColor& Color);
	void SetBorderVis(bool bVis);
	void SetVis(bool bVis);
	
	void Reset();

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	
};
