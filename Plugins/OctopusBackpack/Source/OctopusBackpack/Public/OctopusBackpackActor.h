// Copyright 2024 CAS. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "OctopusTypes.h"

#include "OctopusBackpackActor.generated.h"

UCLASS()
class OCTOPUSBACKPACK_API AOctopusBackpackActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AOctopusBackpackActor();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void OnConstruction(const FTransform& Transform) override;

	//************************************************************************
	// Component                                                                  
	//************************************************************************
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
	class USceneComponent* sceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
	class UStaticMeshComponent* LeftBottomHandStartMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
	class UStaticMeshComponent* RightBottomHandStartMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
	class UStaticMeshComponent* LeftTopHandStartMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
	class UStaticMeshComponent* RightTopHandStartMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
	class UArrowComponent* LeftBottomHandFallingArrow;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
	class UArrowComponent* RightBottomHandFallingArrow;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
	class UArrowComponent* LeftTopHandFallingArrow;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
	class UArrowComponent* RightTopHandFallingArrow;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
	class UArrowComponent* LeftBottomHandStartArrow;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
	class UArrowComponent* RightBottomHandStartArrow;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
	class UArrowComponent* LeftTopHandStartArrow;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
	class UArrowComponent* RightTopHandStartArrow;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
	class UArrowComponent* LeftBottomHandGrabArrow;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
	class UArrowComponent* RightBottomHandGrabArrow;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
	class UArrowComponent* LeftTopHandGrabArrow;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
	class UArrowComponent* RightTopHandGrabArrow;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
	class UArrowComponent* LeftBottomHandWalkArrow;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
	class UArrowComponent* RightBottomHandWalkArrow;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
	class UArrowComponent* LeftTopHandWalkArrow;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
	class UArrowComponent* RightTopHandWalkArrow;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
	class UArrowComponent* LeftBottomHandWalkGrabArrow;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
	class UArrowComponent* RightBottomHandWalkGrabArrow;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
	class UArrowComponent* LeftTopHandWalkGrabArrow;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
	class UArrowComponent* RightTopHandWalkGrabArrow;
	
	//************************************************************************

	void SetStretchLinearLimitsForTentacle(int handIndex, bool bStretch, EOctoHandAction handAction = EOctoHandAction::MOVE_TO_ATTACH_POINT);

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Octopus Backpack | Parameters")
	TArray<FOctoTentaclesStruct> tentaclesArr;

	FTransform GetHandPointTransform(int handID, EOcto_HandPoint handPoint);

	void EnableIdleEffect(int handIndex, bool bEnable);

	void SetSphereParameters(class USphereComponent* sphereComp);
	void SetConstraintParameters(class UPhysicsConstraintComponent* constraintComp);

	void GenerateHandByLength(float length, int handID);

	void ActivateBackpack(bool bActivate);

	UFUNCTION(BlueprintImplementableEvent, Category = "Octopus Backpack | Parameters")
	void ActivateBackpackEvent(bool bActivate);
	UFUNCTION(BlueprintImplementableEvent, Category = "Octopus Backpack | Parameters")
	void GrabbedEvent(FOctoTentaclesStruct tentacle);
	UFUNCTION(BlueprintImplementableEvent, Category = "Octopus Backpack | Parameters")
	void AttackMoveEvent(FOctoTentaclesStruct tentacle);
	UFUNCTION(BlueprintImplementableEvent, Category = "Octopus Backpack | Parameters")
	void MoveToAttachPointEvent(FOctoTentaclesStruct tentacle);
	UFUNCTION(BlueprintImplementableEvent, Category = "Octopus Backpack | Parameters")
	void ReturnEvent(FOctoTentaclesStruct tentacle);
	UFUNCTION(BlueprintImplementableEvent, Category = "Octopus Backpack | Parameters")
	void AttachedEvent(FOctoTentaclesStruct tentacle);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Octopus Backpack | Parameters")
	class UStaticMesh* tentaclesStaticMesh = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Octopus Backpack | Parameters")
	class UStaticMesh* tentaclesBigStaticMesh = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Octopus Backpack | Parameters")
	class USkeletalMesh* handSkeletalMesh = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Octopus Backpack | Parameters")
	class UStaticMesh* junctionStaticMesh = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Octopus Backpack | Parameters")
	class UAnimBlueprint* leftBottomHandAnimationClass = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Octopus Backpack | Parameters")
	class UAnimBlueprint* rightBottomHandAnimationClass = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Octopus Backpack | Parameters")
	class UAnimBlueprint* leftTopHandAnimationClass = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Octopus Backpack | Parameters")
	class UAnimBlueprint* rightTopHandAnimationClass = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Octopus Backpack | Parameters")
	UCurveFloat *walkHandStretchCurve = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Octopus Backpack | Parameters")
	UCurveFloat *attackHandStretchCurve = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Octopus Backpack | Parameters")
	UCurveFloat *handAngularLimitCurve = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Octopus Backpack | Parameters")
	TArray<TSubclassOf <AActor>> effectActorsArr;
	
	UPROPERTY()
	bool bIsHandsShowed = false;

	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void GenerateHands();
	void GenerateOneHand(FOctoTentaclesStruct &tentacles, class UStaticMeshComponent* handMesh, EOcto_Hand hand, int handIndex);
	void ClearHands();
	void HideHandsTick(const float &deltaTime);
	void ShowHandsTick(const float &deltaTime);
	void SetVisibleHands(bool bVisible);
	void GenerateJunction(const int &handIndex, const class USplineComponent *splineComp);	
	

	UPROPERTY()
	int sectionCount = 25;
	UPROPERTY()
	float sphereRadius = 10.f;
	UPROPERTY()
	float sphereSpacer = 2.f;
	UPROPERTY()
	float sphereMass = 0.01f;
	UPROPERTY()
	int junctionCount = 55.f;
	UPROPERTY()
	float junctionEndSpace = 5.f;
	UPROPERTY()
	float maxBrokenHandDistance = 5000.f;
	UPROPERTY()
	float maxBrokenHandDistanceSquared = 0.f;
	UPROPERTY()
	float physDamping_ = 3.f; //10.f;

private:

	UPROPERTY()
	bool bIsActivateBackpack = false;
	UPROPERTY()
	float sphereDistanceLoc = 0.f;
	UPROPERTY()
	class UOctopusBackpackComponent* backpackComponent = nullptr;
	
	
};
