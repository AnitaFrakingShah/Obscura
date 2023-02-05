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
	ACommandUnit();

protected:
	//Called when the game starts OR this unit is spawned
	virtual void BeginPlay() override;

public:
	//Called every frame
	virtual void Tick(float deltaTime) override;

	//Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* playerInputComponent) override;

private:
	//Movement controls
	void moveForward(float value);
	void moveRight(float value);

	//Camera Control
	void rotateCameraPitch(float value);
	void rotateCameraYaw(float value);
	void switchCameras();
	void startBlend(float start, float end);

	bool mCameraIsBlending = false;
	float mCameraGoalLength = 0.0f;

	UPROPERTY(VisibleAnywhere)
	bool mFirstPersonMode = false;

	UPROPERTY(EditAnywhere)
	float mBlendLerp = 0.2f;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* mCamera;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* mCameraArm;
};