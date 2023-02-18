#include "CustomCharacterMovementComponent.h"
#include "CollisionShape.h"
#include "util/enum/MovementModes.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"

UCustomCharacterMovementComponent::UCustomCharacterMovementComponent() {
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

bool UCustomCharacterMovementComponent::isClimbing() const {
	return MovementMode == EMovementMode::MOVE_Custom && CustomMovementMode == ECustomMovementMode::CLIMBING;
}

FVector UCustomCharacterMovementComponent::getClimbSurfaceNormal() const {
	return mCurrentClimbingNormal;
}

void UCustomCharacterMovementComponent::handleClimbInput() {
	if(mIsClimbing) {
		mIsClimbing = false;
	}
	else if(_canStartClimbing()) {
		mIsClimbing = true;
	}
}

void UCustomCharacterMovementComponent::BeginPlay() {
	Super::BeginPlay();
	mClimbQueryParams.AddIgnoredActor(GetOwner());
}

void UCustomCharacterMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	_sweepAndStoreWallHits();
}

void UCustomCharacterMovementComponent::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) {
	if(mIsClimbing) {
		SetMovementMode(EMovementMode::MOVE_Custom, ECustomMovementMode::CLIMBING);
	}
	Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);
}

void UCustomCharacterMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) {
	if(isClimbing()) {
		bOrientRotationToMovement = false;

		UCapsuleComponent* Capsule = CharacterOwner->GetCapsuleComponent();
		Capsule->SetCapsuleHalfHeight(Capsule->GetUnscaledCapsuleHalfHeight() / 1.5f);
	}

	const bool wasClimbing = (PreviousMovementMode == MOVE_Custom && PreviousCustomMode == ECustomMovementMode::CLIMBING);
	if(wasClimbing) {
		bOrientRotationToMovement = true;

		UCapsuleComponent* Capsule = CharacterOwner->GetCapsuleComponent();
		Capsule->SetCapsuleHalfHeight(Capsule->GetUnscaledCapsuleHalfHeight() * 1.5f);

		StopMovementImmediately();
	}

	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);
}

float UCustomCharacterMovementComponent::GetMaxSpeed() const {
	return isClimbing() ? mMaxClimbingSpeed : Super::GetMaxSpeed();
}

float UCustomCharacterMovementComponent::GetMaxAcceleration() const {
	return isClimbing() ? mMaxClimbingAcceleration : Super::GetMaxAcceleration();
}

void UCustomCharacterMovementComponent::PhysCustom(float deltaTime, int32 Iterations) {
	if(CustomMovementMode == ECustomMovementMode::CLIMBING) {
		_physicsForClimbing(deltaTime, Iterations);
	}
	Super::PhysCustom(deltaTime, Iterations);
}

void UCustomCharacterMovementComponent::_sweepAndStoreWallHits() {
	FCollisionShape collisionShape = FCollisionShape::MakeCapsule(mCollisionCapsuleRadius, mCollisionCapsuleHalfHeight);
	FVector start = UpdatedComponent->GetComponentLocation() + (UpdatedComponent->GetForwardVector() * 20.0f);
	FVector end = start + UpdatedComponent->GetForwardVector();

	mWallHitsScratchpad.Reset();
	GetWorld()->SweepMultiByChannel(mWallHitsScratchpad, start, end, FQuat::Identity, ECC_WorldStatic, collisionShape, mClimbQueryParams);
}

bool UCustomCharacterMovementComponent::_canStartClimbing() {
	for(const FHitResult& hit : mWallHitsScratchpad) {
		//This is assuming our up vector for the character is always in the positive Z
		const FVector projectedXYNormal = hit.Normal.GetSafeNormal2D();
		float horizontalAngle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(UpdatedComponent->GetForwardVector(), -projectedXYNormal)));
		const bool isCeiling = FMath::IsNearlyZero(projectedXYNormal.Length());
		if(_facingSurface() && !isCeiling && horizontalAngle <= mMinHorizontalDegrees) { //_facingSurface() 
			return true;
		}
	}
	return false;
}

bool UCustomCharacterMovementComponent::_facingSurface() {
	return _eyeHeightTrace();
}

bool UCustomCharacterMovementComponent::_eyeHeightTrace() {
	FHitResult eyeTraceHit;

	const float baseEyeHeight = GetCharacterOwner()->BaseEyeHeight;
	const FVector start = UpdatedComponent->GetComponentLocation() + (UpdatedComponent->GetUpVector() * baseEyeHeight);
	const FVector end = start + (UpdatedComponent->GetForwardVector() * 200.0f);
	return GetWorld()->LineTraceSingleByChannel(eyeTraceHit, start, end, ECC_WorldStatic, mClimbQueryParams);
}

void UCustomCharacterMovementComponent::_physicsForClimbing(float deltaTime, int32 Iterations) {
	if(deltaTime < MIN_TICK_TIME) {
		return;
	}
	_computeSurfaceInfo();
	if(_shouldStopClimbing()) {
		_stopClimbing(deltaTime, Iterations);
		return;
	}

	_computeClimbingVelocity(deltaTime);

	const FVector OldLocation = UpdatedComponent->GetComponentLocation();

	_moveAlongClimbingSurface(deltaTime);
	if (!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
	{
		Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / deltaTime;
	}
	_snapToClimbingSurface(deltaTime);
}

void UCustomCharacterMovementComponent::_computeSurfaceInfo() {
	for(const FHitResult& hit : mWallHitsScratchpad) {
		mCurrentClimbingNormal += hit.Normal;
		mCurrentClimbingPosition += hit.ImpactPoint;
	}

	mCurrentClimbingNormal = mCurrentClimbingNormal.GetSafeNormal();
	mCurrentClimbingPosition /= mWallHitsScratchpad.Num();
}

bool UCustomCharacterMovementComponent::_shouldStopClimbing() {
	const bool isCeiling = FVector::Parallel(mCurrentClimbingNormal, FVector::UpVector);
	return isCeiling || !mIsClimbing || mCurrentClimbingNormal.IsZero();
}

void UCustomCharacterMovementComponent::_stopClimbing(float deltaTime, int32 Iterations) {
	mIsClimbing = false;
	SetMovementMode(EMovementMode::MOVE_Falling);
	StartNewPhysics(deltaTime, Iterations);
}

void UCustomCharacterMovementComponent::_computeClimbingVelocity(float deltaTime) {
	RestorePreAdditiveRootMotionVelocity();
	if (!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity()) {
		const float friction = 0.0f;
		const bool isFluid = false;
		CalcVelocity(deltaTime, friction, isFluid, mClimbingDeceleration);
	}
}

void UCustomCharacterMovementComponent::_moveAlongClimbingSurface(float deltaTime) {
	const FVector adjusted = Velocity * deltaTime;
	FHitResult hit(1.f);
	SafeMoveUpdatedComponent(adjusted, _getClimbingRotation(deltaTime), true, hit);
	if(hit.Time < 1.f) {
		HandleImpact(hit, deltaTime, adjusted);
		SlideAlongSurface(adjusted, (1.f - hit.Time), hit.Normal, hit, true);
	}
}

FQuat UCustomCharacterMovementComponent::_getClimbingRotation(float deltaTime) {
	const FQuat currentRotation = UpdatedComponent->GetComponentQuat();
	const FQuat targetRotation = FRotationMatrix::MakeFromX(-mCurrentClimbingNormal).ToQuat();
	if (HasAnimRootMotion() || CurrentRootMotion.HasOverrideVelocity()) {
		return currentRotation;
	}
	return FMath::QInterpTo(currentRotation, targetRotation, deltaTime, 6.0f);
}

void UCustomCharacterMovementComponent::_snapToClimbingSurface(float deltaTime) {
	const FVector forward = UpdatedComponent->GetForwardVector();
	const FVector location = UpdatedComponent->GetComponentLocation();
	const FQuat rotation = UpdatedComponent->GetComponentQuat();

	const FVector forwardDifference = (mCurrentClimbingPosition - location).ProjectOnTo(forward);
	const FVector offset = mCurrentClimbingNormal * (forwardDifference.Length() - mGoalDistanceFromSurface);

	UpdatedComponent->MoveComponent(offset * mClimbingSnapSpeed * deltaTime, rotation, true);
}