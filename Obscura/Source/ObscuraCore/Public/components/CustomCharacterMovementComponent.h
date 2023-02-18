#pragma once
 
#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CustomCharacterMovementComponent.generated.h"

UCLASS()
class UCustomCharacterMovementComponent : public UCharacterMovementComponent {
GENERATED_BODY()

public:
	UCustomCharacterMovementComponent();
	bool isClimbing() const;
	FVector getClimbSurfaceNormal() const;
	void handleClimbInput();

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
	bool _canStartClimbing();
	bool _facingSurface();
	bool _eyeHeightTrace();
	void _computeSurfaceInfo();
	bool _shouldStopClimbing();
	void _stopClimbing(float deltaTime, int32 Iterations);
	void _computeClimbingVelocity(float deltaTime);
	void _moveAlongClimbingSurface(float deltaTime);
	void _snapToClimbingSurface(float deltaTime);
	FQuat _getClimbingRotation(float deltaTime);

	UPROPERTY(Category="Character Movement: Climbing", EditAnywhere, meta=(ClampMin="10.0", ClampMax="500.0"))
	float mMaxClimbingSpeed = 120.f;

	UPROPERTY(Category="Character Movement: Climbing", EditAnywhere, meta=(ClampMin="10.0", ClampMax="2000.0"))
	float mMaxClimbingAcceleration = 380.f;

	UPROPERTY(Category="Character Movement: Climbing", EditAnywhere, meta=(ClampMin="10.0", ClampMax="2000.0"))
	float mClimbingDeceleration = 550.0f;

	UPROPERTY(Category="Character Movement: Climbing", EditAnywhere, meta=(ClampMin="15.0", ClampMax="75.0"))
	float mMinHorizontalDegrees = 25.0f;

	int mCollisionCapsuleRadius = 50;
	int mCollisionCapsuleHalfHeight = 72;
	struct FCollisionQueryParams mClimbQueryParams;

	bool mIsClimbing = false;
	FVector mCurrentClimbingNormal;
	FVector mCurrentClimbingPosition;
	class TArray<struct FHitResult> mWallHitsScratchpad;
	float mGoalDistanceFromSurface = 45.0f;
	float mClimbingSnapSpeed = 3.5f;
};