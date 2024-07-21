//
// (C) Copyright 2024 Dirk Michael
// dirkmichaelnm@gmail.com
//

#include "TerrainCameraMovementComponent.h"

#include "Kismet/GameplayStatics.h"

// Defines the log category of this class.
DEFINE_LOG_CATEGORY(TerrainCameraMovement)

/**
 * Default constructor.
 */
UTerrainCameraMovementComponent::UTerrainCameraMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;

	// Set default values
	bDisableCameraMovement = false;
	DefaultMovementSpeed = 15.0;
	DefaultRotationSpeed = 0.5;
	DefaultZoomLength = 280.0;
	DeltaArm = 10.0;
	DeltaRotation = FRotator(5.0, 0.0, 0.0);
	MaxZoom = 400.0;
	MinZoom = 100.0;
	CameraOwner = nullptr;
	PlayerController = nullptr;
}

/**
 * Called when the game starts or when spawned.
 */
void UTerrainCameraMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	// Get the owner
	CameraOwner = Cast<ATerrainCameraPawn>(GetOwner());
	// Get the player controller
	PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
}

/**
 * Called every frame.
 */
void UTerrainCameraMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                    FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

/**
 * Returns the camera location.
 * 
 * @return Camera location vector.
 */
FVector UTerrainCameraMovementComponent::GetCameraLocation() const
{
	return CameraOwner->GetActorLocation();
}

/**
 * Returns the camera direction.
 * 
 * @return Camera direction vector.
 */
FVector UTerrainCameraMovementComponent::GetCameraDirection() const
{
	return CameraOwner->GetActorForwardVector();
}

/**
 * Returns the camera roation.
 * 
 * @return Camera rotation.
 */
FRotator UTerrainCameraMovementComponent::GetCameraRotation() const
{
	return CameraOwner->GetActorRotation();
}

/**
 * Returns a 2D vector containing the current mouse position.
 * 
 * @return 2D vector. 
 */
FVector2D UTerrainCameraMovementComponent::GetCurrentMousePosition() const
{
	auto X{0.0};
	auto Y{0.0};
	PlayerController->GetMousePosition(X, Y);
	return FVector2D(X, Y);
}

/**
 * Set the new movement speed value.
 * 
 * @param MovementSpeedAdjustment Movement speed value.
 * 
 * @return The new movement speed value. 
 */
double UTerrainCameraMovementComponent::SetMovementSpeed(const double MovementSpeedAdjustment)
{
	return DefaultMovementSpeed = MovementSpeedAdjustment;
}

/**
 * Disables or enabled the camera movement.
 * 
 * @param bDisableCamera <b>true</b>, if camera should be disabled, <b>false</b> if camera should be enabled.
 * 
 * @return Camera disabled flag. 
 */
bool UTerrainCameraMovementComponent::SetCameraDisabled(const bool bDisableCamera)
{
	return bDisableCameraMovement = bDisableCamera;
}

/**
 * Controls the movement of the camera with the key W, A, S, D.
 * 
 * @param AxisValueX Delta X movement value.
 * @param AxisValueY Delta Y movement value.
 */
void UTerrainCameraMovementComponent::BasicMovementControl(const double AxisValueX, const double AxisValueY)
{
	// Move only, if movement is enabled
	if (!IsCameraDisabled())
	{
		// Create movement vector
		const auto Movement = FVector(
			AxisValueX * GetCurrentMovementSpeed(),
			AxisValueY * GetCurrentMovementSpeed(),
			0.0
		);

		// Apply movement to camera
		CameraOwner->AddActorLocalOffset(Movement, true);
		// Get new location
		const auto Location = CameraOwner->GetActorLocation();
		// Get terrain size
		const auto [MinimalX, MinimalY, MaximalX, MaximalY] = CameraOwner->GetTerrainSize();
		// Clamp location
		const auto X = Location.X < MinimalX ? MinimalX : Location.X > MaximalX ? MaximalX : Location.X;
		const auto Y = Location.Y < MinimalY ? MinimalY : Location.Y > MaximalY ? MaximalY : Location.Y;
		// Apply new clamped location
		CameraOwner->SetActorLocation(FVector(X, Y, Location.Z));
	}
}

/**
 * Rotates the camera by the given amount.
 * 
 * @param RotationAmount Rotation amount.
 */
void UTerrainCameraMovementComponent::PanCamera(const double RotationAmount)
{
	if (!IsCameraDisabled())
	{
		// Calculate new rotation and apply it
		const auto NewRotation = GetCameraRotation().Add(0.0, RotationAmount * DefaultRotationSpeed, 0.0);
		CameraOwner->SetActorRotation(NewRotation);
	}
}

/**
 * Zooms in.
 */
void UTerrainCameraMovementComponent::ZoomIn()
{
	if (!IsCameraDisabled())
	{
		if (CameraOwner->GetCurrentZoomLength() > MinZoom)
		{
			CameraOwner->UpdateZoomLength(-1.0 * DeltaArm);
			if (CameraOwner->GetCurrentZoomLength() <= DefaultZoomLength)
			{
				CameraOwner->UpdateRotation(DeltaRotation);
			}
		}
	}
}

/**
 * Zooms out.
 */
void UTerrainCameraMovementComponent::ZoomOut()
{
	if (!IsCameraDisabled())
	{
		if (CameraOwner->GetCurrentZoomLength() < MaxZoom)
		{
			CameraOwner->UpdateZoomLength(DeltaArm);
			if (CameraOwner->GetCurrentZoomLength() <= DefaultZoomLength)
			{
				CameraOwner->UpdateRotation(-1.0 * DeltaRotation);
			}
		}
	}
}
