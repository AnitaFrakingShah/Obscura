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

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float deltaTime, enum ELevelTick tickType, FActorComponentTickFunction* func) override;

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> mWidgetClass;

	UPROPERTY(EditAnywhere)
	class ACommander* mCommander;

	class UIconWidget* mIconWidget;
	bool mIconAddedToViewport = false;
};