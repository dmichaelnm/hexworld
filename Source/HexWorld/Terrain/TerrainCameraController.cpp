//
// (C) Copyright 2024 Dirk Michael
// dirkmichaelnm@gmail.com
//

// ReSharper disable CppMemberFunctionMayBeConst

#include "TerrainCameraController.h"

#include "TerrainCameraPawn.h"

/**
 * Called when the game starts or when spawned.
 */
void ATerrainCameraController::BeginPlay()
{
	const auto CameraPawn = Cast<ATerrainCameraPawn>(GetPawn());
	CameraMovement = CameraPawn->CameraMovement;
	SetShowMouseCursor(true);
	RightMouseButtonPressed = false;
}

/**
 * Setups the input component.
 */
void ATerrainCameraController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAxis("MoveForward", this, &ATerrainCameraController::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ATerrainCameraController::MoveRight);
	InputComponent->BindAction("ZoomIn", IE_Pressed, this, &ATerrainCameraController::ZoomIn);
	InputComponent->BindAction("ZoomOut", IE_Pressed, this, &ATerrainCameraController::ZoomOut);
	InputComponent->BindAction("PanCameraLeft", IE_Pressed, this, &ATerrainCameraController::PanCameraLeft);
	InputComponent->BindAction("PanCameraRight", IE_Pressed, this, &ATerrainCameraController::PanCameraRight);

	InputComponent->BindAction("MiddleMouseButton", IE_Pressed, this,
	                           &ATerrainCameraController::SetMiddleMouseButtonPressed);
	InputComponent->BindAction("MiddleMouseButton", IE_Released, this,
	                           &ATerrainCameraController::SetMiddleMouseButtonReleased);
	InputComponent->BindAction("RightMouseButton", IE_Pressed, this,
	                           &ATerrainCameraController::SetRightMouseButtonPressed);
	InputComponent->BindAction("RightMouseButton", IE_Released, this,
	                           &ATerrainCameraController::SetRightMouseButtonReleased);
}

/**
 * Binds to the input event "Move Forward".
 * 
 * @param Value The event value. 
 */
void ATerrainCameraController::MoveForward(const float Value)
{
	if (MiddleMouseButtonPressed && !RightMouseButtonPressed)
	{
		CameraMovement->BasicMovementControl(Value, 0.0);
	}
}

/**
 * Binds to the input event "Move Right".
 * 
 * @param Value The event value. 
 */
void ATerrainCameraController::MoveRight(const float Value)
{
	if (MiddleMouseButtonPressed && !RightMouseButtonPressed)
	{
		CameraMovement->BasicMovementControl(0.0, Value);
	}
	else if (!MiddleMouseButtonPressed && RightMouseButtonPressed)
	{
		const auto MousePosition = CameraMovement->GetCurrentMousePosition();
		const auto Difference = MousePosition.X - CurrentMousePosition.X;
		CurrentMousePosition = MousePosition;
		if (Difference != 0)
		{
			CameraMovement->PanCamera(Difference);
		}
	}
}

/**
 * Zooms in.
 */
void ATerrainCameraController::ZoomIn()
{
	if (!MiddleMouseButtonPressed && !RightMouseButtonPressed)
	{
		CameraMovement->ZoomIn();
	}
}

/**
 * Zooms out.
 */
void ATerrainCameraController::ZoomOut()
{
	if (!MiddleMouseButtonPressed && !RightMouseButtonPressed)
	{
		CameraMovement->ZoomOut();
	}
}

/**
 * Rotate the camera view to the left.
 */
void ATerrainCameraController::PanCameraLeft()
{
	if (!MiddleMouseButtonPressed && !RightMouseButtonPressed)
	{
		CameraMovement->PanCamera(-15.0);
	}
}

/**
 * Rotate the camera view to the right.
 */
void ATerrainCameraController::PanCameraRight()
{
	if (!MiddleMouseButtonPressed && !RightMouseButtonPressed)
	{
		CameraMovement->PanCamera(15.0);
	}
}

/**
 * Set the flag to <b>true</b>, when the middle mouse button is pressed.
 */
void ATerrainCameraController::SetMiddleMouseButtonPressed()
{
	MiddleMouseButtonPressed = true;
}

/**
 * Set the flag to <b>false</b>, when the middle mouse button is released.
 */
void ATerrainCameraController::SetMiddleMouseButtonReleased()
{
	MiddleMouseButtonPressed = false;
}

/**
 * Set the flag to <b>true</b>, when the right mouse button is pressed.
 */
void ATerrainCameraController::SetRightMouseButtonPressed()
{
	// Get current mouse position
	CurrentMousePosition = CameraMovement->GetCurrentMousePosition();
	RightMouseButtonPressed = true;
}

/**
 * Set the flag to <b>false</b>, when the right mouse button is released.
 */
void ATerrainCameraController::SetRightMouseButtonReleased()
{
	RightMouseButtonPressed = false;
}
