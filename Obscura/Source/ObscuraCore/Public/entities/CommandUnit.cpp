#include "CommandUnit.h"
#include "Components/InputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "components/IconComponent.h"
#include "components/CustomCharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "entities/Commander.h"
#include "Kismet/KismetMathLibrary.h"

#include "Logging/LogMacros.h"

ACommandUnit::ACommandUnit(const FObjectInitializer& ObjectInitializer) 
: Super(ObjectInitializer.SetDefaultSubobjectClass<UCustomCharacterMovementComponent>(ACharacter::CharacterMovementComponentName)){
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

	// Movement component reference capture
	MovementComponent = Cast<UCustomCharacterMovementComponent>(GetCharacterMovement());
}

void ACommandUnit::BeginPlay() {
	Super::BeginPlay();
	OnClicked.AddUniqueDynamic(this, &ACommandUnit::onSelected);
}

void ACommandUnit::Tick(float deltaTime) {
	Super::Tick(deltaTime);
	updateBlend();
	updateIconComponent();
	if(!MovementComponent->isClimbing()) {
		updateSpeed();
		rotateActor();
	}
}

void ACommandUnit::SetupPlayerInputComponent(UInputComponent* playerInputComponent) {
	Super::SetupPlayerInputComponent(playerInputComponent);
	setMovementBinds();
	setCameraBinds();
	setActionBinds();
}

///////////////////////////////////////////////////////////////// Movement Functions ////////////////////////////////////
void ACommandUnit::updateSpeed() {
	FVector input = mLastInput.GetUnsafeNormal2D() * mCurrentSpeed;
	AddMovementInput(input);
	if(input.Length() == 0.0f) {
		mCurrentSpeed = 0.1f;
	}
	else {
		mCurrentSpeed = UKismetMathLibrary::Lerp(mCurrentSpeed, mGoalSpeed, mBlendLerp);
	}
}

void ACommandUnit::rotateActor() {
	FVector input = mLastInput.GetUnsafeNormal2D();
	FVector forwardMovement = GetActorForwardVector().GetUnsafeNormal2D();

	float inputAngle = UKismetMathLibrary::Atan2(input.Y,input.X)* 57.2958f;
	float forwardMovementAngle = UKismetMathLibrary::Atan2(forwardMovement.Y,forwardMovement.X)* 57.2958f;
	float angle = inputAngle - forwardMovementAngle;
	angle = angle > 180 ?  - (360 - angle) : angle;
	angle = angle < -180 ? (360 + angle) : angle; 

	if(input.Length() > 0.0f) {
		AddActorWorldRotation(FRotator(0.0f, UKismetMathLibrary::Lerp(0.0f, angle, mBlendLerp), 0.0f));
	}
	mLastInput = FVector::ZeroVector;
}

void ACommandUnit::moveForward(float value) {
	if(MovementComponent->isClimbing()) {
		FVector direction = FVector::CrossProduct(MovementComponent->getClimbSurfaceNormal(), -GetActorRightVector());
		AddMovementInput(direction, value);
	}
	else{
		mLastInput += mCamera->GetForwardVector().GetUnsafeNormal2D() * value;
	}
}

void ACommandUnit::moveRight(float value) {	
	if(MovementComponent->isClimbing()) {
		FVector direction = FVector::CrossProduct(MovementComponent->getClimbSurfaceNormal(), GetActorUpVector());
		AddMovementInput(direction, value);
	}
	else{
		mLastInput += mCamera->GetRightVector().GetUnsafeNormal2D() * value;
	}
}

void ACommandUnit::toggleSprint() {	
	mGoalSpeed = mGoalSpeed == mSprintSpeed ? mWalkSpeed : mSprintSpeed;
}

void ACommandUnit::jump() {
	Jump();
}

void ACommandUnit::climb() {
	MovementComponent->handleClimbInput();
}


void ACommandUnit::setMovementBinds() {
	InputComponent->BindAxis("moveForward", this, &ACommandUnit::moveForward);
	InputComponent->BindAxis("moveRight", this, &ACommandUnit::moveRight);
	InputComponent->BindAxis("moveRight", this, &ACommandUnit::moveRight);
	InputComponent->BindAction("toggleSprint", IE_Pressed, this, &ACommandUnit::toggleSprint);
	InputComponent->BindAction("jump", IE_Pressed, this, &ACommandUnit::jump);
	InputComponent->BindAction("climb", IE_Pressed, this, &ACommandUnit::climb);
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
	UE_LOG(LogTemp, Warning, TEXT("This was clicked"));
	setPlayerController();
}

void ACommandUnit::setPlayerController() {
	APlayerController* controller = Cast<APlayerController>(mOwningCommander->GetController());
	if(controller) {
		FInputModeGameOnly gameMode = FInputModeGameOnly();
		controller->UnPossess();
		controller->Possess(this);

		controller->SetInputMode(gameMode);
		controller->bShowMouseCursor = false;
		controller->bEnableClickEvents = false;
		controller->bEnableMouseOverEvents = false;

		mOwningCommander->HideMap();
	}

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