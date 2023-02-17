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
	FVector getClimbSurfaceNormal();
	void handleClimbInput() const;

private:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;
	virtual float GetMaxSpeed() const override;
	virtual float GetMaxAcceleration() const override;
	virtual void PhysCustom(float deltaTime, int32 Iterations) override;

	UPROPERTY(Category="Character Movement: Climbing", EditAnywhere, meta=(ClampMin="10.0", ClampMax="500.0"))
	float mMaxClimbingSpeed = 120.f;

	UPROPERTY(Category="Character Movement: Climbing", EditAnywhere, meta=(ClampMin="10.0", ClampMax="2000.0"))
	float mMaxClimbingAcceleration = 380.f;

	bool mIsClimbing = false;
	FVector mCurrentClimbingNormal;
	FVector mCurrentClimbingPosition;
};