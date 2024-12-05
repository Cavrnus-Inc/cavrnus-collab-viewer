// Copyright (c) 2024 Cavrnus. All rights reserved.

#include "UI/Components/CavrnusInputField.h"

#include "Components/Border.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"

void UCavrnusInputField::NativeConstruct()
{
	Super::NativeConstruct();

	InputField->OnTextChanged.AddDynamic(this, &UCavrnusInputField::FieldUpdated);
	InputField->OnTextCommitted.AddDynamic(this, &UCavrnusInputField::FieldCommitted);

	SetState(Normal);
}

void UCavrnusInputField::NativeDestruct()
{
	Super::NativeDestruct();
	
	InputField->OnTextChanged.RemoveDynamic(this, &UCavrnusInputField::FieldUpdated);
	InputField->OnTextCommitted.RemoveDynamic(this, &UCavrnusInputField::FieldCommitted);
}

bool UCavrnusInputField::HasAnyText()
{
	return !InputField->GetText().IsEmptyOrWhitespace();
}

void UCavrnusInputField::ClearText()
{
	InputField->SetText(FText::GetEmpty());
}

FString UCavrnusInputField::GetInputString()
{
	return InputField->GetText().ToString();
}

void UCavrnusInputField::SetState(const EInputFieldState NewState)
{
	switch (NewState) {
	case Normal:
		SetBorderVis(false);
		break;
	case Valid:
		SetFieldColor(ValidColor);
		break;
	case Invalid:
		SetFieldColor(InvalidColor);
		break;
	case Disabled:
		SetBorderVis(false);
		break;
	default: ;
	}
	
	CurrentState = NewState;
}

void UCavrnusInputField::SetMessage(const FString& Message)
{
	MessageText->SetText(FText::FromString(Message));
}

void UCavrnusInputField::FieldUpdated(const FText& Text)
{
	OnInputUpdated.Broadcast(Text.ToString());
}

void UCavrnusInputField::FieldCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	OnInputCommitted.Broadcast(Text.ToString(), CommitMethod);
}

void UCavrnusInputField::SetFieldColor(const FLinearColor& Color)
{
	if (MessageText)
		MessageText->SetColorAndOpacity(Color);

	if (InputFieldBorder)
		InputFieldBorder->SetBrushColor(Color);
}

void UCavrnusInputField::SetBorderVis(const bool bVis)
{
	InputFieldBorder->SetVisibility(bVis ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Hidden);
}

void UCavrnusInputField::SetVis(const bool bVis)
{
	InputField->SetVisibility(bVis ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Hidden);
}

void UCavrnusInputField::Reset()
{
	SetState(Normal);
	ClearText();
}