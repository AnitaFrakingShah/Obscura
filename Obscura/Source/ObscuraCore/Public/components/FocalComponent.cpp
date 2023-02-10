#include "FocalComponent.h"
#include "widgets/IconWidget.h"
#include "world/map/Dimensions.h"
#include "Kismet/GameplayStatics.h"
#include "entities/Commander.h"

UFocalComponent::UFocalComponent() {
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

UFocalComponent::UFocalComponent(const FObjectInitializer& ObjectInitializer) {
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UFocalComponent::TickComponent(float deltaTime, enum ELevelTick tickType, FActorComponentTickFunction* func) {
	Super::TickComponent(deltaTime, tickType, func);
	if(mIsActive) {
		mIconWidget->UpdateIconPostion(0.0f, 0.0f, mIconAddedToViewport);
	}
	else {
		mIconWidget->RemoveFromParent();
	}
}

void UFocalComponent::ShowWidget(bool active) {
	mIsActive = active;
}

void UFocalComponent::BeginPlay() {
	Super::BeginPlay();

	//Create Icon Widget and keep reference
	mIconWidget = CreateWidget<UIconWidget>(UGameplayStatics::GetGameInstance(GetWorld()), mWidgetClass);
}