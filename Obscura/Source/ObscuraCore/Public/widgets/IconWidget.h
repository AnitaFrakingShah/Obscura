#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "IconWidget.generated.h"

UCLASS()
class UIconWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Icon Widget Update")
	void UpdateIconPostion(float x, float y, bool& iconAddedToViewport);


	UFUNCTION(BlueprintImplementableEvent, Category = "Icon Widget Update")
	void UpdateIconRotation(float yaw);
};