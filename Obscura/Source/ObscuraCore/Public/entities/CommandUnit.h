#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CommandUnit.generated.h"

class UCameraComponent;
class USpringArmComponent;

UCLASS()
class ACommandUnit : public ACharacter
{
	GENERATED_BODY()

public:
	
	// Sets default values for a command unit
	ACommandUnit(const FObjectInitializer& ObjectInitializer);

	//Called every frame
	virtual void Tick(float deltaTime) override;

	//Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* playerInputComponent) override;

	UFUNCTION(BlueprintPure)
	class UCustomCharacterMovementComponent* GetCustomCharacterMovement() const { return MovementComponent; }

protected:
	//Called when the game starts OR this unit is spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(Category=Character, VisibleAnywhere, BlueprintReadOnly)
	class UCustomCharacterMovementComponent* MovementComponent;

private:

	///////////////////////////////////////////////////////////////// Movement Functions ////////////////////////////////////
	void moveForward(float value);
	void moveRight(float value);
	void toggleSprint();
	void jump();
	void climb();
	void setMovementBinds();
	void updateSpeed();
	void rotateActor();

	//////////////////////////////////////////////////////////////////      Actions      /////////////////////////////////////////
	void switchToCommander();
	void switchCameras();
	void setActionBinds();

	///////////////////////////////////////////////////////////////// Camera Functions ////////////////////////////////////
	void rotateCameraPitch(float value);
	void rotateCameraYaw(float value);
	void startBlend(float start, float end);
	void updateBlend();
	void setCameraBinds();

	///////////////////////////////////////////////////////////////// Player Controller Functions ////////////////////////////////////
	UFUNCTION()
	void onSelected(AActor* Target, FKey ButtonPressed);
	void setPlayerController();

	//////////////////////////////////////////////////////////////// Component Maintenance Functions /////////////////////////////////
	void updateIconComponent();

	////////////////////////////////////////////////////////////////// Blueprint Components /////////////////////////////////////////
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* mCamera;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* mCameraArm;

	UPROPERTY(EditAnywhere)
	class UIconComponent* mIconComponent;

	UPROPERTY(VisibleAnywhere)
	bool mFirstPersonMode = false;

	////////////////////////////////////////////////////////////////// Blueprint Variables /////////////////////////////////////////

	UPROPERTY(EditAnywhere)
	float mBlendLerp = 0.2f;

	UPROPERTY(EditAnywhere)
	float mSprintSpeed = 1.5f;

	UPROPERTY(EditAnywhere)
	float mWalkSpeed = 0.5f;

	UPROPERTY(EditAnywhere)
	class ACommander* mOwningCommander;

	////////////////////////////////////////////////////////////////// Private Class Parameters /////////////////////////////////////////


	float mCurrentSpeed = 0.1f;
	float mGoalSpeed = 0.1f;
	bool mCameraIsBlending = false;
	float mCameraGoalLength = 0.0f;
	FVector mLastInput = FVector::ZeroVector;
};