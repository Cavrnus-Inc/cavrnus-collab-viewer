// // Copyright (c) 2025 Cavrnus. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ProgressBar.h"
#include "UI/Systems/Messages/CavrnusBaseUIMessageWidget.h"
#include "CavrnusBaseToastMessageWidget.generated.h"

UCLASS(Abstract)
class CAVRNUSCONNECTOR_API UCavrnusBaseToastMessageWidget : public UCavrnusBaseUIMessageWidget
{
	GENERATED_BODY()
public:
	UPROPERTY()
	bool Hovered = false;

	void StartTimer(float InDuration);
	void StopTimer();
protected:
	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UProgressBar> ProgressBar = nullptr;

	bool TimerActive = false;
	float ElapsedTime = 0;
	float Duration = 0;

	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
};
