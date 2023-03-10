#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "IconComponent.generated.h"

UCLASS()
class UIconComponent : public UActorComponent {

	GENERATED_BODY()
public:
	UIconComponent();
	UIconComponent(const FObjectInitializer& InObjectInitializer);
	void ShowWidget(bool active);
	virtual void TickComponent(float deltaTime, enum ELevelTick tickType, FActorComponentTickFunction* func) override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> mWidgetClass;
	
	UPROPERTY()
	class UIconWidget* mIconWidget;

	UPROPERTY(EditAnywhere)
	class UMaterialParameterCollection* mMaterialParamCollection;

	UPROPERTY()
	class UMaterialParameterCollectionInstance* mMaterialParamInstance;

	bool mIconAddedToViewport = false;
	bool mIsActive = false;
};