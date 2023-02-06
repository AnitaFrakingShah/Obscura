#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Commander.generated.h"

UCLASS()
class ACommander : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACommander();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//Movement and Rotation
	void moveForward(float value);
	void moveRight(float value);
	void moveUp(float value);
	void rotateCameraYaw(float value);
	void rotateCameraPitch(float value);

	//Set Map Parameters
	void setMapParameters();

	//Bluepring Parameters
	UPROPERTY(VisibleAnywhere)
	class UCameraComponent* mCamera;

	UPROPERTY(VisibleAnywhere)
	class USpringArmComponent* mCameraArm;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* mBoxCollision;

	UPROPERTY(EditAnywhere)
	class USkeletalMeshComponent* mSkeletalMesh;

	UPROPERTY(EditAnywhere)
	class UUserWidget* mCommanderWidgetRef;

	UPROPERTY(EditAnywhere)
	class UMaterialParameterCollection* mMaterialParamCollection;

	UPROPERTY(EditAnywhere)
	class UFloatingPawnMovement* mFloatingComponent;

	UPROPERTY(EditAnywhere)
	class UIconComponent* mIconComponent;

private:
	class UMaterialParameterCollectionInstance* mMaterialParamInstance;
};