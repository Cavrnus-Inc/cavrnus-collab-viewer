// Copyright (c) 2025 Cavrnus. All rights reserved.

#include "UI/Systems/Messages/ToastMessages/CavrnusBaseToastMessageWidget.h"

void UCavrnusBaseToastMessageWidget::StartTimer(const float InDuration)
{
	Duration = InDuration;
	TimerActive = true;

	if (ProgressBar)
		ProgressBar->SetVisibility(ESlateVisibility::Visible);
}

void UCavrnusBaseToastMessageWidget::StopTimer()
{
	TimerActive = false;

	if (ProgressBar)
		ProgressBar->SetVisibility(ESlateVisibility::Hidden);
}

void UCavrnusBaseToastMessageWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	Hovered = true;
	StopTimer();
}

void UCavrnusBaseToastMessageWidget::NativeTick(const FGeometry& MyGeometry, const float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!TimerActive & ProgressBar != nullptr)
		return;

	if (ElapsedTime < Duration)
	{
		ElapsedTime += InDeltaTime;
		const float Percent = FMath::Clamp(ElapsedTime / Duration, 0.f, 1.f);
		
		ProgressBar->SetPercent(1 - Percent);
	}
}
