#include "CommandUnit.h"
#include "Components/InputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

ACommandUnit::ACommandUnit(){
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	bUseControllerRotationYaw = false;
	mCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	mCameraArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));

	mCameraArm->SetupAttachment(RootComponent);
	mCameraArm->TargetArmLength = 500.0f;
	mCameraArm->SetRelativeRotation(FRotator(-45.0f, 0.0f, 0.0f));

	mCameraArm->bEnableCameraLag = true;
	mCameraArm->CameraLagSpeed = 2;
	mCameraArm->CameraLagMaxDistance = 1;

	mCameraArm->bEnableCameraRotationLag = true;
	mCameraArm->CameraRotationLagSpeed = 4;
	mCameraArm->CameraLagMaxTimeStep = 1;

	mCamera->SetupAttachment(mCameraArm, USpringArmComponent::SocketName);
}

void ACommandUnit::BeginPlay() {
	Super::BeginPlay();
}

void ACommandUnit::Tick(float deltaTime) {
	Super::Tick(deltaTime);
	if(mCameraIsBlending) {
		mCameraArm->TargetArmLength = FMath::Lerp(mCameraArm->TargetArmLength, mCameraGoalLength, mBlendLerp);
		if(mCameraArm->TargetArmLength == mCameraGoalLength) {
			mCameraIsBlending = false;
		}
	}
}

void ACommandUnit::SetupPlayerInputComponent(UInputComponent* playerInputComponent) {
	Super::SetupPlayerInputComponent(playerInputComponent);

	InputComponent->BindAxis("rotateCameraYaw", this, &ACommandUnit::rotateCameraYaw);
	InputComponent->BindAxis("rotateCameraPitch", this, &ACommandUnit::rotateCameraPitch);
	InputComponent->BindAxis("moveForward", this, &ACommandUnit::moveForward);
	InputComponent->BindAxis("moveRight", this, &ACommandUnit::moveRight);

	InputComponent->BindAction("switchCameras", IE_Pressed, this, &ACommandUnit::switchCameras);
}

void ACommandUnit::startBlend(float start, float end) {
	mCameraIsBlending = true;
	mCameraGoalLength = end;
}

void ACommandUnit::switchCameras() {
	if(mFirstPersonMode) {
		startBlend(0.0f, 500.f);
	}
	else {
		startBlend(500.0f, 0.f);
	}
	mFirstPersonMode = !mFirstPersonMode;
}

void ACommandUnit::rotateCameraPitch(float value) {
	if(value) {
		float newPitchRotation = mCameraArm->GetRelativeRotation().Pitch + value;
		if(newPitchRotation < 25 && newPitchRotation > -65) {
			mCameraArm->AddLocalRotation(FRotator(value, 0.0f, 0.0f));
		}
	}
}

void ACommandUnit::rotateCameraYaw(float value) {
	if(value) {
		mCameraArm->AddWorldRotation(FRotator(0.0f, value, 0.0f));
	}
}

void ACommandUnit::moveForward(float value) {
	//Rotate our actor to face direction of movement
	float directionYaw = mCameraArm->GetRelativeRotation().Yaw;
	FRotator actorRotation = GetActorRotation();
	if(value > 0) {
		actorRotation.Yaw = std::fmod(directionYaw, 360.0f);
	}
	else if(value < 0) {
		actorRotation.Yaw = std::fmod((directionYaw + 180.0f), 360.0f);
	}
	SetActorRotation(actorRotation);

	//Move actor in forward direction
	AddMovementInput(GetActorForwardVector() * FMath::Abs(value));
}

void ACommandUnit::moveRight(float value) {	
	//Rotate our actor to face direction of movement
	float directionYaw = mCameraArm->GetRelativeRotation().Yaw;
	FRotator actorRotation = GetActorRotation();
	if(value > 0) {
		actorRotation.Yaw = std::fmod((directionYaw + 90.0f), 360.0f);
	}
	else if(value < 0) {
		actorRotation.Yaw = std::fmod((directionYaw - 90.0f), 360.0f);
	}

	SetActorRotation(actorRotation);

	//Move actor in forward direction
	AddMovementInput(GetActorForwardVector() * FMath::Abs(value));
}
