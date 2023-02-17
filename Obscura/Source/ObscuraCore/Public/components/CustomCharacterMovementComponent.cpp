#include "CustomCharacterMovementComponent.h"

UCustomCharacterMovementComponent::UCustomCharacterMovementComponent() {

}

bool UCustomCharacterMovementComponent::isClimbing() const {
	return mIsClimbing;
}

FVector UCustomCharacterMovementComponent::getClimbSurfaceNormal() {
	return mCurrentClimbingNormal;
}

void UCustomCharacterMovementComponent::handleClimbInput() const {

}