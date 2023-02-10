#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FocalComponent.generated.h"

UCLASS()
class UFocalComponent : public UActorComponent {

	GENERATED_BODY()
public:
	UFocalComponent();
	UFocalComponent(const FObjectInitializer& InObjectInitializer);
	virtual void TickComponent(float deltaTime, enum ELevelTick tickType, FActorComponentTickFunction* func) override;
	void ShowWidget(bool active);

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> mWidgetClass;

	UPROPERTY()
	class UIconWidget* mIconWidget;
	
	bool mIconAddedToViewport = false;
	bool mIsActive = false;
};