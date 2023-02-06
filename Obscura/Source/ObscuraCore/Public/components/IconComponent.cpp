#include "IconComponent.h"
#include "widgets/IconWidget.h"
#include "world/map/Dimensions.h"
#include "Kismet/GameplayStatics.h"
#include "entities/Commander.h"

UIconComponent::UIconComponent() {
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

UIconComponent::UIconComponent(const FObjectInitializer& ObjectInitializer) {
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UIconComponent::BeginPlay() {
	Super::BeginPlay();

	//Create Icon Widget and keep reference
	mIconWidget = CreateWidget<UIconWidget>(UGameplayStatics::GetGameInstance(GetWorld()), mWidgetClass);
}

void UIconComponent::TickComponent(float deltaTime, enum ELevelTick tickType, FActorComponentTickFunction* func) {
	Super::TickComponent(deltaTime, tickType, func);
	FVector location = GetOwner()->GetActorLocation();
	FVector commanderPosition = mCommander->GetActorLocation();

	// Commander position at point <0,0> map widget
	location -= commanderPosition;
	float x = (location.X * WORLD_MAP_MAX_X_SCALE);
	float y = (location.Y * WORLD_MAP_MAX_Y_SCALE);

	mIconWidget->UpdateIconPostion(x, y, mIconAddedToViewport);

	if(mIconAddedToViewport) {
		mIconWidget->UpdateIconRotation(GetOwner()->GetActorRotation().Yaw);
	}
}