
#include "Commander.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Materials/MaterialParameterCollection.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "GameFramework/SpringArmComponent.h"

ACommander::ACommander() {
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	bUseControllerRotationYaw = false;
	// Create Box Collision
	mBoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collision"));
	SetRootComponent(mBoxCollision);

	mSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeletal Mesh"));
	mSkeletalMesh->SetupAttachment(mBoxCollision);

	mCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	mCameraArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));

	mCameraArm->SetupAttachment(mBoxCollision);
	mCameraArm->TargetArmLength = 1.0f;
	mCameraArm->SetRelativeRotation(FRotator(-45.0f, 0.0f, 0.0f));

	mCameraArm->bEnableCameraLag = true;
	mCameraArm->CameraLagSpeed = 2;
	mCameraArm->CameraLagMaxDistance = 1;

	mCameraArm->bEnableCameraRotationLag = true;
	mCameraArm->CameraRotationLagSpeed = 4;
	mCameraArm->CameraLagMaxTimeStep = 1;

	mCamera->SetupAttachment(mCameraArm, USpringArmComponent::SocketName);
	mFloatingComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("Floating Movement"));
}

// Called when the game starts or when spawned
void ACommander::BeginPlay() {
	Super::BeginPlay();
}

// Called every frame
void ACommander::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ACommander::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) {

	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAxis("rotateCameraYaw", this, &ACommander::rotateCameraYaw);
	InputComponent->BindAxis("rotateCameraPitch", this, &ACommander::rotateCameraPitch);
	InputComponent->BindAxis("moveForward", this, &ACommander::moveForward);
	InputComponent->BindAxis("moveRight", this, &ACommander::moveRight);
	InputComponent->BindAxis("moveUp", this, &ACommander::moveUp);
}

void ACommander::rotateCameraYaw(float value) {
	if(value) {
		AddActorWorldRotation(FRotator(0.0f, value, 0.0f));
	}
}

void ACommander::rotateCameraPitch(float value) {
	if(value) {
		float newPitchRotation = mCameraArm->GetRelativeRotation().Pitch + value;
		if(newPitchRotation < 25 && newPitchRotation > -65) {
			mCameraArm->AddLocalRotation(FRotator(value, 0.0f, 0.0f));
		}
	}
}

void ACommander::moveForward(float value) {
	AddMovementInput(GetActorForwardVector() * value);
}

void ACommander::moveRight(float value) {
	AddMovementInput(GetActorRightVector() * value);
}

void ACommander::moveUp(float value) {
	AddMovementInput(GetActorUpVector() * value);
}
