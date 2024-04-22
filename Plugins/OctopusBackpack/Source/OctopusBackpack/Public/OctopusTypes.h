// Copyright 2024 CAS. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Engine.h"
#include "OctopusTypes.generated.h"

UENUM(BlueprintType)
enum class EOcto_Move: uint8
{
	NONE UMETA(DisplayName = "None"),
	FORWARD UMETA(DisplayName = "Forward"),
	BACKWARD UMETA(DisplayName = "Backward"),
	TOP UMETA(DisplayName = "Top"),
	BOTTOM UMETA(DisplayName = "Bottom"),
	LEFT UMETA(DisplayName = "Left"),
	RIGHT UMETA(DisplayName = "Right")
};

UENUM(BlueprintType)
enum class EOctoHandAction: uint8
{
	IDLE				 UMETA(DisplayName = "Idle"),
	MOVE_TO_ATTACH_POINT UMETA(DisplayName = "Moves towards the attachment point"),
	ATTACHED			 UMETA(DisplayName = "Attached"),
	GRABBED_RETURN		 UMETA(DisplayName = "Grabbed the target and return"),
	ATTACKS				 UMETA(DisplayName = "Attacks the target"),
	RETURNS				 UMETA(DisplayName = "Returns to its place"),
	GRABBED				 UMETA(DisplayName = "Grabbed the target"),
	THROW				 UMETA(DisplayName = "Throw the object"),
	FALLING				 UMETA(DisplayName = "Falling"),
	SHOW_HIDE			 UMETA(DisplayName = "Show Hide")
};

UENUM(BlueprintType)
enum class EOcto_Hand: uint8
{
	LEFT_BOTTOM UMETA(DisplayName = "LeftBottom"),
	RIGHT_BOTTOM UMETA(DisplayName = "RightBottom"),
	LEFT_TOP UMETA(DisplayName = "LeftTop"),
	RIGHT_TOP UMETA(DisplayName = "RightTop")
};

UENUM(BlueprintType)
enum class EOcto_HandPoint: uint8
{
	WALK_ORIGIN UMETA(DisplayName = "Walk"),
	WALK_GRAB_ORIGIN UMETA(DisplayName = "Wlak Grab"),
	FLY_ORIGIN  UMETA(DisplayName = "Fly"),
	FALLING_ORIGIN  UMETA(DisplayName = "Falling"),
	FLY_GRAB_ORIGIN UMETA(DisplayName = "Fly Grab")
};

USTRUCT(BlueprintType)
struct FOctoHandStatus
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Octopus Backpack | Parameters")
	EOcto_Hand hand = EOcto_Hand::LEFT_BOTTOM;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Octopus Backpack | Parameters")
	FTransform handTransform = FTransform::Identity;
	UPROPERTY(EditDefaultsOnly, Category = "Octopus Backpack | Parameters")
	FTransform handBeforeMove = FTransform::Identity;
	UPROPERTY(EditDefaultsOnly, Category = "Octopus Backpack | Parameters")
	float sizeHandNow = 0.f;
	UPROPERTY(EditDefaultsOnly, Category = "Octopus Backpack | Parameters")
	FTransform targetPoint = FTransform::Identity;
	UPROPERTY(EditDefaultsOnly, Category = "Octopus Backpack | Parameters")
	TArray<FVector> targetPointsArr;
	UPROPERTY(EditDefaultsOnly, Category = "Octopus Backpack | Parameters")
	FHitResult targetHitResult;
	UPROPERTY(EditDefaultsOnly, Category = "Octopus Backpack | Parameters")
	FVector checkPoint = FVector::ZeroVector;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Octopus Backpack | Parameters")
	EOctoHandAction action = EOctoHandAction::IDLE;
	UPROPERTY(EditDefaultsOnly, Category = "Octopus Backpack | Parameters")
	double timeAttached = 0.f;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Octopus Backpack | Parameters")
	bool bBusyAlgo = false;
	UPROPERTY()
	float walkMoveHandTime = 0.f;
	UPROPERTY()
	float walkRotateHandTime = 0.f;
	UPROPERTY()
	float throwImpulse = 0.f;
	UPROPERTY()
	bool bFinishAnimate = true;
	UPROPERTY()
	float animRandRate = 0.f;
	UPROPERTY()
	float animTime = 0.f;
	UPROPERTY()
	float animIdleTime = 0.f;
	UPROPERTY()
	bool bCameraAnimation = false;
	UPROPERTY()
	bool bLookInstance = 0.f;
	UPROPERTY()
	UObject* grabbedObject = nullptr;

	//Constructor
	FOctoHandStatus()
	{
	}
};

USTRUCT(BlueprintType)
struct FOctoTentaclesStruct
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Octopus Backpack | Parameters")
	EOcto_Hand hand = EOcto_Hand::LEFT_BOTTOM;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Octopus Backpack | Parameters")
	class USkeletalMeshComponent* handSkeletalComponent = nullptr;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Octopus Backpack | Parameters")
	class UOctopusBackpackComponent* backpackComponent = nullptr;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Octopus Backpack | Parameters")
	int handIndex = -1;
	UPROPERTY()
	TArray<class USphereComponent*> sphereCompsArr;
	UPROPERTY()
	TArray<class USplineMeshComponent*> splineMeshCompsArr;
	UPROPERTY()
	TArray<class USplineMeshComponent*> splineMeshBigCompsArr;
	UPROPERTY()
	TArray<class UPhysicsConstraintComponent*> constraintCompsArr;
	UPROPERTY()
	class USplineComponent* splineComponent = nullptr;
	UPROPERTY()
	float newYawRotation = 0.f;
	UPROPERTY()
	class UArrowComponent* targetArrow = nullptr;
	UPROPERTY()
	class UArrowComponent* fallingArrow = nullptr;
	UPROPERTY()
	class UArrowComponent* grabArrow = nullptr;
	UPROPERTY()
	class UArrowComponent* originArrow = nullptr;
	UPROPERTY()
	class UArrowComponent* walkGrabArrow = nullptr;
	UPROPERTY()
	class UArrowComponent* walkArrow = nullptr;
	UPROPERTY()
	class UInstancedStaticMeshComponent* junctionInstancedComp = nullptr;
	UPROPERTY()
	class UStaticMeshComponent* startHandMesh = nullptr;
	UPROPERTY()
	float splineStartLength = 0.f;
	UPROPERTY()
	float splineCurrentLength = 0.f;
	UPROPERTY()
	float splineHideTime = 0.f;
	UPROPERTY()
	int splineStartPoint = -1;
	UPROPERTY()
	AActor* effectActor = nullptr;

	//Constructor
	FOctoTentaclesStruct()
	{
	}
};