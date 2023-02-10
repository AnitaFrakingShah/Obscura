#include "IconComponent.h"
#include "widgets/IconWidget.h"
#include "world/map/Dimensions.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialParameterCollection.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "entities/Commander.h"

UIconComponent::UIconComponent() {
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

UIconComponent::UIconComponent(const FObjectInitializer& ObjectInitializer) {
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UIconComponent::ShowWidget(bool active) {
	mIsActive = active;
}

void UIconComponent::TickComponent(float deltaTime, enum ELevelTick tickType, FActorComponentTickFunction* func) {
	Super::TickComponent(deltaTime, tickType, func);
	if(mIsActive) {
		FVector location = GetOwner()->GetActorLocation();
		float mapX = 0.0f;
		float mapY = 0.0f;
		mMaterialParamInstance->GetScalarParameterValue("Map_Center_World_X", mapX);
		mMaterialParamInstance->GetScalarParameterValue("Map_Center_World_Y", mapY);

		location -= FVector(mapX, mapY, location.Z);
		
		float x = (location.X /  (MAX_WORLD_X * WORLD_MAP_MAX_Y_SCALE));
		float y = (location.Y /  (MAX_WORLD_Y * WORLD_MAP_MAX_X_SCALE));

		mIconWidget->UpdateIconPostion(y, -x, mIconAddedToViewport);

		if(mIconAddedToViewport) {
			mIconWidget->UpdateIconRotation(GetOwner()->GetActorRotation().Yaw);
		}
	}
	else {
		mIconWidget->RemoveFromParent();
	}
}

void UIconComponent::BeginPlay() {
	Super::BeginPlay();
	if(!mMaterialParamInstance){ // if(mMaterialParamInstance == nullptr)
		mMaterialParamInstance = GetWorld()->GetParameterCollectionInstance(mMaterialParamCollection);
	}
	//Create Icon Widget and keep reference
	mIconWidget = CreateWidget<UIconWidget>(UGameplayStatics::GetGameInstance(GetWorld()), mWidgetClass);
}