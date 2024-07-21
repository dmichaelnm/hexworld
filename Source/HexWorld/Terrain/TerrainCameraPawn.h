//
// (C) Copyright 2024 Dirk Michael
// dirkmichaelnm@gmail.com
//

#pragma once

#include "CoreMinimal.h"
#include "TerrainSize.h"
#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "TerrainCameraPawn.generated.h"

// Defines the log category of this class.
DECLARE_LOG_CATEGORY_EXTERN(TerrainCameraPawn, Log, All);

class UTerrainCameraMovementComponent;

/**
 * This class manages the main camera of the game.
 */
UCLASS()
class HEXWORLD_API ATerrainCameraPawn : public APawn
{
	GENERATED_BODY()

public:
	/**
	 * Default constructor.
	 */
	ATerrainCameraPawn();

protected:
	/**
	 * Called when the game starts or when spawned.
	 */
	virtual void BeginPlay() override;

public:
	/**
	 * Camera movement component.
	 */
	UPROPERTY(VisibleAnywhere, Category = "Terrain Camera", meta=(AllowPrivateAccess = "true"))
	UTerrainCameraMovementComponent* CameraMovement;
	
	/**
	 * Called every frame.
	 */
	virtual void Tick(float DeltaTime) override;

protected:
	/**
	 * Collision sphere
	 */
	UPROPERTY(VisibleAnywhere, Category = "Terrain Camera")
	USphereComponent* CollisionSphere;

	/**
	 * Main camera
	 */
	UPROPERTY(VisibleAnywhere, Category = "Terrain Camera")
	UCameraComponent* Camera;

	/**
	 * Spring arm
	 */
	UPROPERTY(VisibleAnywhere, Category = "Terrain Camera")
	USpringArmComponent* CameraArm;
	
	/**
	 * Default camera arm length
	 */
	UPROPERTY(EditAnywhere, Category = "Terrain Camera")
	double DefaultZoomLength;
	
	/**
	 * Minimum camera rotation
	 */
	UPROPERTY(EditAnywhere, Category = "Terrain Camera")
	FRotator MinimumRotation;

	/**
	 * Maximum camera rotation
	 */
	UPROPERTY(EditAnywhere, Category = "Terrain Camera")
	FRotator MaximumRotation;
	
	/**
	 * Default camera rotation
	 */
	UPROPERTY(EditAnywhere, Category = "Terrain Camera")
	FRotator DefaultRotation;

public:
	/**
	 * Returns the camera component.
	 * 
	 * @return Camera component. 
	 */
	FORCEINLINE UCameraComponent* GetCamera() const { return Camera; }

	/**
	 * Returns the camera spring arm component.
	 * 
	 * @return Spring arm component.
	 */
	FORCEINLINE USpringArmComponent* GetCameraArm() const { return CameraArm; }

	/**
	 * Returns the terrain size.
	 * 
	 * @return Terrain size struct. 
	 */
	FORCEINLINE FTerrainSize GetTerrainSize() const { return TerrainSize; }
	
	/**
	 * Returns the current length of the camera arm.
	 * 
	 * @return Current camera arm length.  
	 */
	double GetCurrentZoomLength() const;

	/**
	 * Returns the current rotation of the camera.
	 * 
	 * @return Current rotation. 
	 */
	FRotator GetCurrentRotation() const;

	/**
	 * Adds the change amount to the current zoom length.
	 * 
	 * @param ChangeAmount The amount to be added. 
	 */
	virtual void UpdateZoomLength(const float ChangeAmount);

	/**
	 * Adds the change amount to the current rotation.
	 * 
	 * @param ChangeAmount The amount to be added. 
	 */
	virtual void UpdateRotation(const FRotator ChangeAmount);

	/**
	 * Set the zoom to the default value.
	 */
	virtual void SetToDefaultZoom();

private:
	/**
	 * Terrain size struct.
	 */
	FTerrainSize TerrainSize;
	
};
