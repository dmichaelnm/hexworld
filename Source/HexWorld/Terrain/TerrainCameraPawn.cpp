//
// (C) Copyright 2024 Dirk Michael
// dirkmichaelnm@gmail.com
//

#include "TerrainCameraPawn.h"

#include "TerrainActor.h"
#include "TerrainCameraMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Defines the log category of this class.
DEFINE_LOG_CATEGORY(TerrainCameraPawn)

/**
 * Default constructor.
 */
ATerrainCameraPawn::ATerrainCameraPawn()
{
	// Set defaults
	DefaultZoomLength = 280.0;
	MinimumRotation = FRotator(-90.0, 0.0, 0.0);
	MaximumRotation = FRotator(-0.0, 0.0, 0.0);
	DefaultRotation = FRotator(-90.0, 0.0, 0.0);

	// Set root component and it's size
	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Collision Sphere"));
	CollisionSphere->InitSphereRadius(32.0);
	CollisionSphere->SetWorldScale3D(FVector(0.25, 0.25, 0.25));
	SetRootComponent(CollisionSphere);

	// Default settings for inheriting controller rotations
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Create and attach camera arm component to root
	CameraArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Arm"));
	CameraArm->SetupAttachment(CollisionSphere);
	CameraArm->SetAbsolute(false, false, false);
	CameraArm->TargetArmLength = DefaultZoomLength;
	CameraArm->SetRelativeRotation(DefaultRotation);
	CameraArm->bDoCollisionTest = true;
	CameraArm->bEnableCameraLag = true; // for smoother movements
	CameraArm->bEnableCameraRotationLag = true;
	CameraArm->bInheritPitch = false; // needs to be false for zoom to be updated in real time

	// Create the camera
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraArm, USpringArmComponent::SocketName);

	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	// Attach camera movement component
	CameraMovement = CreateDefaultSubobject<UTerrainCameraMovementComponent>(TEXT("Camera Movement"));
}

/**
 * Called when the game starts or when spawned.
 */
void ATerrainCameraPawn::BeginPlay()
{
	Super::BeginPlay();

	auto Actors = TArray<AActor*>();
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATerrainActor::StaticClass(), Actors);
	const auto Terrain = static_cast<ATerrainActor*>(Actors[0]);
	if (IsValid(Terrain))
	{
		TerrainSize = Terrain->GetBounds();
		const auto X = TerrainSize.MinimalX + (TerrainSize.MaximalX - TerrainSize.MinimalX) / 2.0;
		const auto Y = TerrainSize.MinimalY + (TerrainSize.MaximalY - TerrainSize.MinimalY) / 2.0;
		UE_LOG(TerrainCameraPawn, Display, TEXT("Terrain Actor found (Location: %f:%f)."), X, Y);

		SetActorLocation(FVector(X, Y, 100.0));
	}
	else
	{
		SetActorLocation(FVector(0, 0, 100.0));
		UE_LOG(TerrainCameraPawn, Warning, TEXT("Terrain Actor not found."));
	}
	SetActorRotation(FRotator::ZeroRotator);
}

/**
 * Called every frame.
 */
void ATerrainCameraPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	auto Forward = GetActorForwardVector();
	Forward.Normalize();
	const auto Upper = FVector(0.0, 0.0, 1.0);
	auto Right = FVector::CrossProduct(Forward, Upper);
	Right.Normalize();
}

/**
 * Returns the current length of the camera arm.
 * 
 * @return Current camera arm length.  
 */
double ATerrainCameraPawn::GetCurrentZoomLength() const
{
	return CameraArm->TargetArmLength;
}

/**
 * Returns the current rotation of the camera.
 * 
 * @return Current rotation. 
 */
FRotator ATerrainCameraPawn::GetCurrentRotation() const
{
	return CameraArm->GetRelativeRotation();
}

/**
 * Adds the change amount to the current zoom length.
 * 
 * @param ChangeAmount The amount to be added. 
 */
void ATerrainCameraPawn::UpdateZoomLength(const float ChangeAmount)
{
	CameraArm->TargetArmLength += ChangeAmount;
}

/**
 * Adds the change amount to the current rotation.
 * 
 * @param ChangeAmount The amount to be added. 
 */
void ATerrainCameraPawn::UpdateRotation(const FRotator ChangeAmount)
{
	// Calculate new rotation
	auto NewRotation = FRotator(GetCurrentRotation() + ChangeAmount);
	// Clamp new rotation to be in the defined boundaries
	NewRotation = NewRotation.Pitch < MinimumRotation.Pitch
		              ? MinimumRotation
		              : NewRotation.Pitch > MaximumRotation.Pitch
		              ? MaximumRotation
		              : FRotator(NewRotation.Pitch, 0.0, 0.0);
	// Set the new rotation
	CameraArm->SetRelativeRotation(NewRotation);
}

/**
 * Set the zoom to the default value.
 */
void ATerrainCameraPawn::SetToDefaultZoom()
{
	CameraArm->TargetArmLength = DefaultZoomLength;
	CameraArm->SetRelativeRotation(DefaultRotation);
}
