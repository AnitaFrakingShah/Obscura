
#include "Commander.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/BoxComponent.h"
#include "components/FocalComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Materials/MaterialParameterCollection.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "GameFramework/SpringArmComponent.h"
#include "world/map/Dimensions.h"

ACommander::ACommander() {
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessPlayer = EAutoReceiveInput::Player0;
	bUseControllerRotationYaw = false;

	// Create Box Collision set as root
	mBoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collision"));
	SetRootComponent(mBoxCollision);

	// Create skeletal mesh component
	mSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeletal Mesh"));
	mSkeletalMesh->SetupAttachment(mBoxCollision);

	//Setup camera and camera components
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

	//Set up floating movement to give "drone" like feel
	mFloatingComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("Floating Movement"));

	//Set up focal component for focal widget on map
	mFocalComponent = CreateDefaultSubobject<UFocalComponent>(TEXT("Focal Component"));
}

// Called when the game starts or when spawned
void ACommander::BeginPlay() {
	Super::BeginPlay();
}

// Called every frame
void ACommander::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	updateMapParameters();
}

// Called to bind functionality to input
void ACommander::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	setMovementBinds();
	setMapParameters();
	setPlayerController();
}

void ACommander::HideMap() {
	mCommanderWidgetRef->RemoveFromParent();
	mFocalComponent->ShowWidget(false);
}

//////////////////////////////////////// PRIVATE MOVEMENT FUNCTIONS //////////////////////////////////////////

void ACommander::moveForward(float value) {
	AddMovementInput(GetActorForwardVector() * value);
}

void ACommander::moveRight(float value) {
	AddMovementInput(GetActorRightVector() * value);
}

void ACommander::setMovementBinds() {
	InputComponent->BindAxis("moveForward", this, &ACommander::moveForward);
	InputComponent->BindAxis("moveRight", this, &ACommander::moveRight);
}

//////////////////////////////////////// PRIVATE CONTROLLER SETUP FUNCTIONS //////////////////////////////////
void ACommander::setPlayerController() {
	// change input mode (mouse is locked, mouse viewable, enable click events, enable hover over events, and interacting with UI elements)
	APlayerController* controller = Cast<APlayerController>(GetController());
	if(controller) {
		FInputModeGameAndUI gameMode = FInputModeGameAndUI();
		gameMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
		gameMode.SetHideCursorDuringCapture(false);

		controller->SetInputMode(gameMode);
		controller->bShowMouseCursor = true;
		controller->bEnableClickEvents = true;
		controller->bEnableMouseOverEvents = true;

		//controller->DefaultClickTraceChannel = ECollisionChannel::ECC_Camera;
	}
}

/////////////////////////////////////// PRIVATE MAP FUNCTIONS ////////////////////////////////////////////////
void ACommander::updateMapParameters() {
	if(!mMaterialParamInstance) {
		return;
	}

	FVector worldPosition = GetActorLocation();
	FVector cameraForward = mCameraArm->GetForwardVector();

	float multiplier = (worldPosition.Z - SURFACE_LEVEL) / cameraForward.Z;
	float X = worldPosition.X - (cameraForward.X * multiplier);
	float Y = worldPosition.Y - (cameraForward.Y * multiplier);

	mMaterialParamInstance->SetScalarParameterValue("Map_X_Coordinate", X / MAX_WORLD_X);
	mMaterialParamInstance->SetScalarParameterValue("Map_Y_Coordinate", Y / MAX_WORLD_Y);
	mMaterialParamInstance->SetScalarParameterValue("Map_Center_World_X", X);
	mMaterialParamInstance->SetScalarParameterValue("Map_Center_World_Y", Y);
}

void ACommander::setMapParameters() {
	if(!mMaterialParamInstance){
		mMaterialParamInstance = GetWorld()->GetParameterCollectionInstance(mMaterialParamCollection);
		mMaterialParamInstance->SetScalarParameterValue("MAX_WORLD_X", MAX_WORLD_X);
		mMaterialParamInstance->SetScalarParameterValue("MAX_WORLD_Y", MAX_WORLD_Y);
		mMaterialParamInstance->SetScalarParameterValue("WORLD_MAP_MAX_X_SCALE", WORLD_MAP_MAX_X_SCALE);
		mMaterialParamInstance->SetScalarParameterValue("SURFACE_LEVEL", SURFACE_LEVEL);
	}
	mCommanderWidgetRef->AddToViewport(0);
	mFocalComponent->ShowWidget(true);
}