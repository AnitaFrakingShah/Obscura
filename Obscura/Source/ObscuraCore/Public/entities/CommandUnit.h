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
	//Camera Control
	void rotateCamera(float value);
	void rotateCameraHorizontal(float value);
	void switchCameras();

	UPROPERTY()
	bool mFirstPersonMode = false;

	UPROPERTY()
	UCameraComponent* mCamera;

	UPROPERTY()
	USpringArmComponent* mCameraArm;
};