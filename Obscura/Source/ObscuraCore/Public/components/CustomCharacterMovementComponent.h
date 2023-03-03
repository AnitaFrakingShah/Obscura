#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CustomCharacterMovementComponent.generated.h"

UCLASS()
class UCustomCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UCustomCharacterMovementComponent();
	void handleClimbInput();

	UFUNCTION(BlueprintPure)
	bool isClimbing() const;

	UFUNCTION(BlueprintPure)
	FVector getClimbSurfaceNormal() const;

private:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;
	virtual float GetMaxSpeed() const override;
	virtual float GetMaxAcceleration() const override;
	virtual void PhysCustom(float deltaTime, int32 Iterations) override;

	void _physicsForClimbing(float deltaTime, int32 Iterations);
	void _sweepAndStoreWallHits();
	bool _eyeHeightTrace(const float traceDistance, bool debugDraw = false) const;
	bool _facingSurface(const float scale) const;
	bool _canStartClimbing() const;


	//Functions for _physicsForClimbing
	void _computeSurfaceInfo();
	void _computeClimbingVelocity(float deltaTime, const bool calculateVelocity);
	bool _shouldStopClimbing();
	void _stopClimbing(float deltaTime, int32 Iterations);
	void _moveAlongClimbingSurface(float deltaTime);
	void _snapToClimbingSurface(float deltaTime) const;
	FQuat _getClimbingRotation(float deltaTime) const;
	void _tryClimbUpLedge() const;
	bool _hasReachedEdge() const;
	bool _canMoveOverEdge() const;
	bool _isLocationWalkable(const FVector& locationToCheck) const;
	

	UPROPERTY(Category="Character Movement: Climbing", EditAnywhere)
	UAnimMontage* LedgeClimbMontage;

	UPROPERTY()
	UAnimInstance* AnimInstance;

	UPROPERTY(Category="Character Movement: Climbing", EditAnywhere, meta=(ClampMin="0.0", ClampMax="80.0"))
	float mClimbingCollisionShrinkAmount = 30;

	UPROPERTY(Category="Character Movement: Climbing", EditAnywhere, meta=(ClampMin="1.0", ClampMax="75.0"))
	float mMinHorizontalDegreesToStartClimbing = 25;

	UPROPERTY(Category="Character Movement: Climbing", EditAnywhere, meta=(ClampMin="0.01", ClampMax="5.0"))
	float mOffset = 0.1f;

	UPROPERTY(Category="Character Movement: Climbing", EditAnywhere, meta=(ClampMin="10.0", ClampMax="500.0"))
	float mMaxClimbingSpeed = 120.f;

	UPROPERTY(Category="Character Movement: Climbing", EditAnywhere, meta=(ClampMin="10.0", ClampMax="2000.0"))
	float mMaxClimbingAcceleration = 380.f;

	UPROPERTY(Category="Character Movement: Climbing", EditAnywhere, meta=(ClampMin="0.0", ClampMax="3000.0"))
	float mBrakingDecelerationClimbing = 550.f;
	
	UPROPERTY(Category="Character Movement: Climbing", EditAnywhere, meta=(ClampMin="1.0", ClampMax="12.0"))
	int mClimbingRotationSpeed = 6;

	UPROPERTY(Category="Character Movement: Climbing", EditAnywhere, meta=(ClampMin="0.0", ClampMax="60.0"))
	float mClimbingSnapSpeed = 4.f;

	UPROPERTY(Category="Character Movement: Climbing", EditAnywhere, meta=(ClampMin="0.0", ClampMax="80.0"))
	float mDistanceFromSurface = 45.f;

	int mCollisionCapsuleRadius = 50;
	int mCollisionCapsuleHalfHeight = 72;
	class TArray<struct FHitResult> mWallHitsScratchpad;
	struct FCollisionQueryParams ClimbQueryParams;

	bool mIsClimbing = false;
	FVector mCurrentClimbingNormal;
	FVector mCurrentClimbingPosition;
};