//
// (C) Copyright 2024 Dirk Michael
// dirkmichaelnm@gmail.com
//

#pragma once

#include "CoreMinimal.h"
#include "TerrainCameraMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "TerrainCameraController.generated.h"

/**
 * Terrain camera controller. 
 */
UCLASS()
class HEXWORLD_API ATerrainCameraController : public APlayerController
{
	GENERATED_BODY()

public:
	/**
     * Called when the game starts or when spawned.
     */
	virtual void BeginPlay() override;

	/**
	 * Setups the input component.
	 */
	virtual void SetupInputComponent() override;

protected:
	/**
	 * Camera movement component.
	 */
	UPROPERTY(VisibleAnywhere, Category = "Camera Controller")
	UTerrainCameraMovementComponent* CameraMovement;
	
	/**
	 * Binds to the input event "Move Forward".
	 * 
	 * @param Value The event value. 
	 */
	virtual void MoveForward(const float Value);

	/**
	 * Binds to the input event "Move Right".
	 * 
	 * @param Value The event value. 
	 */
	virtual void MoveRight(const float Value);

	/**
	 * Zooms in.
	 */
	virtual void ZoomIn();

	/**
	 * Zooms out.
	 */
	virtual void ZoomOut();

	/**
	 * Rotate the camera view to the left.
	 */
	virtual void PanCameraLeft();

	/**
	 * Rotate the camera view to the right.
	 */
	virtual void PanCameraRight();

	/**
	 * Set the flag to <b>true</b>, when the middle mouse button is pressed.
	 */
	virtual void SetMiddleMouseButtonPressed();

	/**
	 * Set the flag to <b>false</b>, when the middle mouse button is released.
	 */
	virtual void SetMiddleMouseButtonReleased();

	/**
	 * Set the flag to <b>true</b>, when the right mouse button is pressed.
	 */
	virtual void SetRightMouseButtonPressed();

	/**
	 * Set the flag to <b>false</b>, when the right mouse button is released.
	 */
	virtual void SetRightMouseButtonReleased();
	
private:
	/**
	 * Flag for a pressed middle mouse button.
	 */
	bool MiddleMouseButtonPressed;
	
	/**
	 * Flag for a pressed right mouse button.
	 */
	bool RightMouseButtonPressed;

	/**
	 * Current mouse position.
	 */
	FVector2D CurrentMousePosition;
};
