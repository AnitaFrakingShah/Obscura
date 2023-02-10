#include "CommandUnit.h"
#include "Components/InputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "components/IconComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "entities/Commander.h"

#include "Logging/LogMacros.h"

ACommandUnit::ACommandUnit(){
	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationYaw = false;

	// Camera creation and setup
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

	// Icon component setup and creation
	mIconComponent = CreateDefaultSubobject<UIconComponent>(TEXT("Icon Component"));
}

void ACommandUnit::BeginPlay() {
	Super::BeginPlay();
	OnClicked.AddUniqueDynamic(this, &ACommandUnit::onSelected);
}

void ACommandUnit::Tick(float deltaTime) {
	Super::Tick(deltaTime);
	updateBlend();
	updateIconComponent();
}

void ACommandUnit::SetupPlayerInputComponent(UInputComponent* playerInputComponent) {
	Super::SetupPlayerInputComponent(playerInputComponent);
	setMovementBinds();
	setCameraBinds();
	setActionBinds();
}

///////////////////////////////////////////////////////////////// Movement Functions ////////////////////////////////////
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

void ACommandUnit::setMovementBinds() {
	InputComponent->BindAxis("moveForward", this, &ACommandUnit::moveForward);
	InputComponent->BindAxis("moveRight", this, &ACommandUnit::moveRight);
}

//////////////////////////////////////////////////////////////////      Actions      /////////////////////////////////////////
void ACommandUnit::switchCameras() {
	if(mFirstPersonMode) {
		startBlend(0.0f, 500.f);
		mCameraArm->bInheritYaw = false;
	}
	else {
		startBlend(500.0f, 0.f);
		mCameraArm->bInheritYaw = true;
	}
	mFirstPersonMode = !mFirstPersonMode;
}

void ACommandUnit::switchToCommander() {
	APlayerController* controller = Cast<APlayerController>(GetController());
	if(controller) {
		controller->UnPossess();
		controller->Possess(mOwningCommander);
	}
}

void ACommandUnit::setActionBinds() {
	//Disable switch cameras till I get the movement fixed
	//InputComponent->BindAction("switchCameras", IE_Pressed, this, &ACommandUnit::switchCameras);
	InputComponent->BindAction("switchToCommander", IE_Pressed, this, &ACommandUnit::switchToCommander);
}

///////////////////////////////////////////////////////////////// Camera Functions ////////////////////////////////////
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
		if(!mFirstPersonMode) {
			mCameraArm->AddWorldRotation(FRotator(0.0f, value, 0.0f));
		}
		else {
			AddActorWorldRotation(FRotator(0.0f, value, 0.0f));
		}
	}
}

void ACommandUnit::startBlend(float start, float end) {
	mCameraIsBlending = true;
	mCameraGoalLength = end;
}

void ACommandUnit::updateBlend() {
	if(mCameraIsBlending) {
		mCameraArm->TargetArmLength = FMath::Lerp(mCameraArm->TargetArmLength, mCameraGoalLength, mBlendLerp);
		if(mCameraArm->TargetArmLength == mCameraGoalLength) {
			mCameraIsBlending = false;
		}
	}
}

void ACommandUnit::setCameraBinds() {
	InputComponent->BindAxis("rotateCameraYaw", this, &ACommandUnit::rotateCameraYaw);
	InputComponent->BindAxis("rotateCameraPitch", this, &ACommandUnit::rotateCameraPitch);
}

///////////////////////////////////////////////////////////////// Player Controller Functions ////////////////////////////////////
void ACommandUnit::onSelected(AActor* Target, FKey ButtonPressed) {
	setPlayerController();
}

void ACommandUnit::setPlayerController() {
}

///////////////////////////////////////////////////////////////// Component Maintenance Functions ////////////////////////////////////
void ACommandUnit::updateIconComponent() {
	if(mOwningCommander->GetController()) {
		mIconComponent->ShowWidget(true);
	}
	else {
		mIconComponent->ShowWidget(false);
	}
}