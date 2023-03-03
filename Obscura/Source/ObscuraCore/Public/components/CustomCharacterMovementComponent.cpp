#include "CustomCharacterMovementComponent.h" 
#include "CollisionShape.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "util/enum/MovementModes.h"

UCustomCharacterMovementComponent::UCustomCharacterMovementComponent() {
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UCustomCharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	ClimbQueryParams.AddIgnoredActor(GetOwner());
	AnimInstance = GetCharacterOwner()->GetMesh()->GetAnimInstance();
}

void UCustomCharacterMovementComponent::handleClimbInput() {
	if(mIsClimbing) {
		mIsClimbing = false;
	}
	else if(_canStartClimbing()) {
		mIsClimbing = true;
	}
}

bool UCustomCharacterMovementComponent::isClimbing() const
{
	return MovementMode == EMovementMode::MOVE_Custom && CustomMovementMode == ECustomMovementMode::CLIMBING;
}

FVector UCustomCharacterMovementComponent::getClimbSurfaceNormal() const {
	return mCurrentClimbingNormal;
}

void UCustomCharacterMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	_sweepAndStoreWallHits();
}

void UCustomCharacterMovementComponent::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) {
	if (mIsClimbing)
	{
		SetMovementMode(EMovementMode::MOVE_Custom, ECustomMovementMode::CLIMBING);
	}

	Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);
}

void UCustomCharacterMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) {
	if (isClimbing())
	{
		bOrientRotationToMovement = false;

		UCapsuleComponent* Capsule = CharacterOwner->GetCapsuleComponent();
		Capsule->SetCapsuleHalfHeight(Capsule->GetUnscaledCapsuleHalfHeight() - mClimbingCollisionShrinkAmount);
	}

	const bool wasClimbing = PreviousMovementMode == MOVE_Custom && PreviousCustomMode == ECustomMovementMode::CLIMBING;
	if (wasClimbing)
	{
		bOrientRotationToMovement = true;

		const FRotator standRotation = FRotator(0,  UpdatedComponent->GetComponentRotation().Yaw, 0);
		UpdatedComponent->SetRelativeRotation(standRotation);

		UCapsuleComponent* Capsule = CharacterOwner->GetCapsuleComponent();
		Capsule->SetCapsuleHalfHeight(Capsule->GetUnscaledCapsuleHalfHeight() + mClimbingCollisionShrinkAmount);

		// After exiting climbing mode, reset velocity and acceleration
		StopMovementImmediately();
	}

	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);
}

float UCustomCharacterMovementComponent::GetMaxSpeed() const
{
	return isClimbing() ? mMaxClimbingSpeed : Super::GetMaxSpeed();
}

float UCustomCharacterMovementComponent::GetMaxAcceleration() const
{
	return isClimbing() ? mMaxClimbingAcceleration : Super::GetMaxAcceleration();
}

void UCustomCharacterMovementComponent::PhysCustom(float deltaTime, int32 Iterations) {
	if (CustomMovementMode == ECustomMovementMode::CLIMBING) {
		_physicsForClimbing(deltaTime, Iterations);
	}

	Super::PhysCustom(deltaTime, Iterations);
}

// Functions for physics climbing
void UCustomCharacterMovementComponent::_computeSurfaceInfo() {
	mCurrentClimbingNormal = FVector::ZeroVector;
	mCurrentClimbingPosition = FVector::ZeroVector;


	const FVector start = UpdatedComponent->GetComponentLocation();


	for (const FHitResult& hit : mWallHitsScratchpad) {
		FHitResult fineTuneHit;

		const FVector end = start + ((hit.ImpactPoint - start).GetSafeNormal() * 120);
	 	GetWorld()->LineTraceSingleByChannel(fineTuneHit, start, end, ECC_WorldStatic, ClimbQueryParams);
		mCurrentClimbingNormal += fineTuneHit.Normal;
		mCurrentClimbingPosition += fineTuneHit.ImpactPoint;
	}

	mCurrentClimbingPosition /= mWallHitsScratchpad.Num();
	mCurrentClimbingNormal = mCurrentClimbingNormal.GetSafeNormal();
}

void UCustomCharacterMovementComponent::_physicsForClimbing(float deltaTime, int32 Iterations) {
	if (deltaTime < MIN_TICK_TIME)
	{
		return;
	}

	bool animationInControl = HasAnimRootMotion() || CurrentRootMotion.HasOverrideVelocity();
	if(!animationInControl) {
		_computeSurfaceInfo();
	}

	if (_shouldStopClimbing())
	{
		_stopClimbing(deltaTime, Iterations);
		return;
	}

	_computeClimbingVelocity(deltaTime, !animationInControl);

	const FVector OldLocation = UpdatedComponent->GetComponentLocation();

	_moveAlongClimbingSurface(deltaTime);
	_tryClimbUpLedge();

	if (!animationInControl)
	{
		Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / deltaTime;
	}

	_snapToClimbingSurface(deltaTime);

}

void UCustomCharacterMovementComponent::_computeClimbingVelocity(float deltaTime, const bool calculateVelocity) {
	RestorePreAdditiveRootMotionVelocity();
	if (calculateVelocity) {
		constexpr float friction = 0.0f;
		constexpr bool isFluid = false;
		CalcVelocity(deltaTime, friction, isFluid, mBrakingDecelerationClimbing);
	}
	ApplyRootMotionToVelocity(deltaTime);
}

bool UCustomCharacterMovementComponent::_shouldStopClimbing() {
	const bool isOnCeiling = FVector::Parallel(mCurrentClimbingNormal, FVector::UpVector);
	return !mIsClimbing || mCurrentClimbingNormal.IsZero() || isOnCeiling;
}

void UCustomCharacterMovementComponent::_stopClimbing(float deltaTime, int32 Iterations) {
	mIsClimbing = false;
	SetMovementMode(EMovementMode::MOVE_Falling);
	StartNewPhysics(deltaTime, Iterations);
	
}

void UCustomCharacterMovementComponent::_moveAlongClimbingSurface(float deltaTime) {
	const FVector adjusted = Velocity * deltaTime;

	FHitResult hit(1.f);

	SafeMoveUpdatedComponent(adjusted, _getClimbingRotation(deltaTime), true, hit);

	if (hit.Time < 1.f)
	{
		HandleImpact(hit, deltaTime, adjusted);
		SlideAlongSurface(adjusted, (1.f - hit.Time), hit.Normal, hit, true);
	}
}

void UCustomCharacterMovementComponent::_snapToClimbingSurface(float deltaTime) const {
	const FVector forward = UpdatedComponent->GetForwardVector();
	const FVector location = UpdatedComponent->GetComponentLocation();
	const FQuat rotation = UpdatedComponent->GetComponentQuat();

	const FVector forwardDifference = (mCurrentClimbingPosition - location).ProjectOnTo(forward);
	const FVector offset = -mCurrentClimbingNormal * (forwardDifference.Length() - mDistanceFromSurface);

	constexpr bool sweep = true;
	UpdatedComponent->MoveComponent(offset * mClimbingSnapSpeed * deltaTime, rotation, sweep);
}

FQuat UCustomCharacterMovementComponent::_getClimbingRotation(float deltaTime) const {
	const FQuat current = UpdatedComponent->GetComponentQuat();
	const FQuat target = FRotationMatrix::MakeFromX(-mCurrentClimbingNormal).ToQuat();
	if (HasAnimRootMotion() || CurrentRootMotion.HasOverrideVelocity())
	{
			return current;
	}
	return FMath::QInterpTo(current, target, deltaTime, mClimbingRotationSpeed);
}

// Functions to identify when we should climb
void UCustomCharacterMovementComponent::_sweepAndStoreWallHits() {
	const FCollisionShape collisionShape = FCollisionShape::MakeCapsule(mCollisionCapsuleRadius, mCollisionCapsuleHalfHeight);
	const FVector startOffset = UpdatedComponent->GetForwardVector() * 20;

	const FVector start = UpdatedComponent->GetComponentLocation() + startOffset;
	const FVector end = start + UpdatedComponent->GetForwardVector();

	mWallHitsScratchpad.Reset();
	GetWorld()->SweepMultiByChannel(mWallHitsScratchpad, start, end, FQuat::Identity, ECC_WorldStatic, collisionShape, ClimbQueryParams);
}

bool UCustomCharacterMovementComponent::_eyeHeightTrace(const float traceDistance, bool debugDraw) const { 
	FHitResult eyeLevelHit;

	const float BaseEyeHeight = GetCharacterOwner()->BaseEyeHeight;
	const float EyeHeightOffset = isClimbing() ? BaseEyeHeight + mClimbingCollisionShrinkAmount : BaseEyeHeight;

	const FVector start = UpdatedComponent->GetComponentLocation() + UpdatedComponent->GetUpVector() * EyeHeightOffset;
	
	const FVector end = start + (UpdatedComponent->GetForwardVector() * traceDistance);

	bool hitSomething = GetWorld()->LineTraceSingleByChannel(eyeLevelHit, start, end, ECC_WorldStatic, ClimbQueryParams);
	if(debugDraw) {
			DrawDebugLine(
					GetWorld(),
					start,
					eyeLevelHit.Location,
					FColor(255, 255, 255),
					false, 15, 0,
					12.3);
	}
	return hitSomething;
}

bool UCustomCharacterMovementComponent::_facingSurface(const float scale) const { 
	return _eyeHeightTrace(mCollisionCapsuleHalfHeight * (1 + (1 - scale)));
}

bool UCustomCharacterMovementComponent::_canStartClimbing() const { 
	for(const FHitResult& hit : mWallHitsScratchpad) {
		const FVector projectedXYNormal = hit.Normal.GetSafeNormal2D(); //Projects onto XY plane which cancels out any Z amount.

		const float horizontalDegrees = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(UpdatedComponent->GetForwardVector(), -projectedXYNormal)));
		const float lineTraceScale = projectedXYNormal.Length();
		const bool bIsCeiling = FMath::IsNearlyZero(projectedXYNormal.Length());
		if(_facingSurface(lineTraceScale) && !bIsCeiling && horizontalDegrees <= mMinHorizontalDegreesToStartClimbing) {
			return true;
		}
	}
	return false;
}

void UCustomCharacterMovementComponent::_tryClimbUpLedge() const {
	if(AnimInstance && LedgeClimbMontage && AnimInstance->Montage_IsPlaying(LedgeClimbMontage)) {
		return;
	}

	const float upSpeed = FVector::DotProduct(Velocity, UpdatedComponent->GetUpVector());
	const bool isMovingUp = upSpeed >= mMaxClimbingSpeed / 4.0f;

	if(isMovingUp  && _hasReachedEdge() && _canMoveOverEdge()) {
		const FRotator standRotation = FRotator(0.0f, UpdatedComponent->GetComponentRotation().Yaw, 0.0f);
		UpdatedComponent->SetRelativeRotation(standRotation);

		AnimInstance->Montage_Play(LedgeClimbMontage);
	}
}


bool UCustomCharacterMovementComponent::_hasReachedEdge() const {
	const UCapsuleComponent* capsule = CharacterOwner->GetCapsuleComponent();
	const float traceDistance = capsule->GetUnscaledCapsuleRadius() * 2.5f;

	return !_eyeHeightTrace(traceDistance, false);
}


bool UCustomCharacterMovementComponent::_canMoveOverEdge() const {
	const UCapsuleComponent* capsule = CharacterOwner->GetCapsuleComponent();
	const FVector verticalOffset = capsule->GetScaledCapsuleHalfHeight() * 2.0f * FVector::UpVector;
	const FVector horizontalOffset = capsule->GetUnscaledCapsuleRadius() * 2.5f * UpdatedComponent->GetForwardVector();

	const FVector locationToCheck = UpdatedComponent->GetComponentLocation() + verticalOffset + horizontalOffset;

	if(!_isLocationWalkable(locationToCheck)) {
		return false;
	}

	FHitResult CapsuleHit;
	const FVector start = locationToCheck - horizontalOffset;
	const bool isBlocked = GetWorld()->SweepSingleByChannel(CapsuleHit, start, locationToCheck, 
		FQuat::Identity, ECC_WorldStatic, capsule->GetCollisionShape(), ClimbQueryParams);

	return !isBlocked;
}

bool UCustomCharacterMovementComponent::_isLocationWalkable(const FVector& locationToCheck) const {
	const FVector end = locationToCheck + (FVector::DownVector * 250.0f);
	FHitResult groundHit;

	const bool hitGround = GetWorld()->LineTraceSingleByChannel(groundHit, locationToCheck, end, 
		ECC_WorldStatic, ClimbQueryParams);

	return hitGround && groundHit.Normal.Z >= GetWalkableFloorZ();
} 