#include "CommandUnit.h"
#include "Components/InputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

ACommandUnit::ACommandUnit(){
}

void ACommandUnit::BeginPlay() {
	Super::BeginPlay();
}

void ACommandUnit::Tick(float deltaTime) {}

void ACommandUnit::SetupPlayerInputComponent(UInputComponent* playerInputComponent) {
	Super::SetupPlayerInputComponent(playerInputComponent);
}

void ACommandUnit::switchCameras() {
}

void ACommandUnit::rotateCamera(float value) {
}

void ACommandUnit::rotateCameraHorizontal(float value) {
}
