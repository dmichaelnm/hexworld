//
// (C) Copyright 2024 Dirk Michael
// dirkmichaelnm@gmail.com
//

#pragma once

#include "CoreMinimal.h"
#include "TerrainCameraPawn.h"
#include "Components/ActorComponent.h"
#include "TerrainCameraMovementComponent.generated.h"

// Defines the log category of this class.
DECLARE_LOG_CATEGORY_EXTERN(TerrainCameraMovement, Log, All);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HEXWORLD_API UTerrainCameraMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	/**
	 * Default constructor.
	 */
	UTerrainCameraMovementComponent();

protected:
	/**
	 * Called when the game starts or when spawned.
	 */
	virtual void BeginPlay() override;

public:	
	/**
	 * Called every frame.
	 */
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/**
	 * The attached pawn
	 */
	UPROPERTY(Transient, EditAnywhere, Category = "Camera Movement")
	ATerrainCameraPawn* CameraOwner;

	/**
	 * Player controller reference 
	 */
	UPROPERTY(Transient, EditAnywhere, Category = "Camera Movement")
	APlayerController* PlayerController;

protected:
	/**
	 * Camera movement disabled.
	 */
	UPROPERTY(EditAnywhere, Category = "Camera Movement")
	bool bDisableCameraMovement;
	
	/**
	 * Camera movement speed.
	 */
	UPROPERTY(EditAnywhere, Category = "Camera Movement")
	double DefaultMovementSpeed;

	/**
	 * Camera rotation speed.
	 */
	UPROPERTY(EditAnywhere, Category = "Camera Movement")
	double DefaultRotationSpeed;

	/**
	 * Default zoom length.
	 */
	UPROPERTY(EditAnywhere, Category = "Camera Movement")
	double DefaultZoomLength;

	/**
	 * Amount of camera arm changes.
	 */
	UPROPERTY(EditAnywhere, Category = "Camera Movement")
	double DeltaArm;

	/**
	 * Amount of camera rotation changes.
	 */
	UPROPERTY(EditAnywhere, Category = "Camera Movement")
	FRotator DeltaRotation;

	/**
	 * Minimum zoom.
	 */
	UPROPERTY(EditAnywhere, Category = "Camera Movement")
	float MinZoom;

	/**
	 * Maximum zoom.
	 */
	UPROPERTY(EditAnywhere, Category = "Camera Movement")
	float MaxZoom;

public:
	/**
	 * Returns the current movement speed.
	 * 
	 * @return Movement speed value. 
	 */
	FORCEINLINE double GetCurrentMovementSpeed() const { return DefaultMovementSpeed; }

	/**
	 * Returns <b>true</b>, if camera movement is disabled, otherwise <b>false</b>.
	 * 
	 * @return Camera disabled flag. 
	 */
	FORCEINLINE bool IsCameraDisabled() const { return bDisableCameraMovement; }

	/**
	 * Returns the camera location.
	 * 
	 * @return Camera location vector.
	 */
	FVector GetCameraLocation() const;

	/**
	 * Returns the camera direction.
	 * 
	 * @return Camera direction vector.
	 */
	FVector GetCameraDirection() const;

	/**
	 * Returns the camera roation.
	 * 
	 * @return Camera rotation.
	 */
	FRotator GetCameraRotation() const;

	/**
	 * Returns a 2D vector containing the current mouse position.
	 * 
	 * @return 2D vector. 
	 */
	FVector2D GetCurrentMousePosition() const;
	
	/**
	 * Set the new movement speed value.
	 * 
	 * @param MovementSpeedAdjustment Movement speed value.
	 * 
	 * @return The new movement speed value. 
	 */
	double SetMovementSpeed(const double MovementSpeedAdjustment);

	/**
	 * Disables or enabled the camera movement.
	 * 
	 * @param bDisableCamera <b>true</b>, if camera should be disabled, <b>false</b> if camera should be enabled.
	 * 
	 * @return Camera disabled flag. 
	 */
	bool SetCameraDisabled(const bool bDisableCamera);

	/**
	 * Controls the movement of the camera with the key W, A, S, D.
	 * 
	 * @param AxisValueX Delta X movement value.
	 * @param AxisValueY Delta Y movement value.
	 */
	virtual void BasicMovementControl(const double AxisValueX, const double AxisValueY);

	/**
	 * Rotates the camera by the given amount.
	 * 
	 * @param RotationAmount Rotation amount.
	 */
	virtual void PanCamera(const double RotationAmount);

	/**
	 * Zooms in.
	 */
	virtual void ZoomIn();

	/**
	 * Zooms out.
	 */
	virtual void ZoomOut();
};
