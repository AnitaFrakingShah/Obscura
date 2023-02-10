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

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//Hides the Map Widget
	void HideMap();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:	
	//////////////////////////////////////////////////   Movement Functions
	void moveForward(float value);
	void moveRight(float value);
	void setMovementBinds();

	/////////////////////////////////////////////////   Map functions
	void setMapParameters();
	void updateMapParameters();

	/////////////////////////////////////////////////   Input Functions for game mode
	void setPlayerController();

	////////////////////////////////////////////////    Parameters needed for map updates
	UPROPERTY()
	class UMaterialParameterCollectionInstance* mMaterialParamInstance;

	UPROPERTY(EditAnywhere)
	class UMaterialParameterCollection* mMaterialParamCollection;

	UPROPERTY(EditAnywhere)
	class UUserWidget* mCommanderWidgetRef;

	///////////////////////////////////////////////////   Blueprint And Component Parameters
	UPROPERTY(VisibleAnywhere)
	class UCameraComponent* mCamera;

	UPROPERTY(VisibleAnywhere)
	class USpringArmComponent* mCameraArm;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* mBoxCollision;

	UPROPERTY(EditAnywhere)
	class USkeletalMeshComponent* mSkeletalMesh;

	UPROPERTY(EditAnywhere)
	class UFloatingPawnMovement* mFloatingComponent;

	UPROPERTY(EditAnywhere)
	class UFocalComponent* mFocalComponent;
};