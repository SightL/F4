// Copyright 2024 CAS. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "OctopusTypes.h"
#include "Components/ActorComponent.h"
#include "OctopusBackpackComponent.generated.h"


UCLASS(Blueprintable)
class OCTOPUSBACKPACK_API UOctopusBackpackComponent : public UActorComponent
{
	GENERATED_BODY()
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOctopusReturnMovement, FVector, inScale);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOctopusFlyingMode, bool, bIsActivated);

public:	
	// Sets default values for this component's properties
	UOctopusBackpackComponent();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void BroadcastOnOctopusReturnMovement(const FVector& inScale) const;
	UPROPERTY(BlueprintAssignable)
	FOctopusReturnMovement OnOctopusReturnMovement;

	void BroadcastOnOctopusFlyingMode(const bool& bIsActivated) const;
	UPROPERTY(BlueprintAssignable)
	FOctopusFlyingMode OnOctopusFlyingMode;

	UFUNCTION(BlueprintCallable, Category = "Octopus Backpack | Parameters")
	void SetMovementDirection(FVector setInputScale);
	UFUNCTION(BlueprintCallable, Category = "Octopus Backpack | Parameters")
	void OctopusBackpackFlyingMode(bool bActivate);
	UFUNCTION(BlueprintCallable, Category = "Octopus Backpack | Parameters")
	void OctopusBackpackBattleMode(bool bActivate);
	UFUNCTION(BlueprintCallable, Category = "Octopus Backpack | Parameters")
	void RegisterOctopusBackpack();
	UFUNCTION(BlueprintCallable, Category = "Octopus Backpack | Parameters")
	void Attack(FHitResult targetHitResult);
	UFUNCTION(BlueprintCallable, Category = "Octopus Backpack | Parameters")
	void AttackHand(FHitResult targetHitResult, int handIndex);
	UFUNCTION(BlueprintCallable, Category = "Octopus Backpack | Parameters")
	bool IsComponentGrabbed(UObject* checkObject);
	UFUNCTION(BlueprintCallable, Category = "Octopus Backpack | Parameters")
	void Drop(int handIndex);
	UFUNCTION(BlueprintCallable, Category = "Octopus Backpack | Parameters")
	void Throw(int handIndex, FHitResult targetHitResult, float addThrowImpulse);
	UFUNCTION(BlueprintCallable, Category = "Octopus Backpack | Parameters")
	void DropAll();
	UFUNCTION(BlueprintCallable, Category = "Octopus Backpack | Parameters")
	void ThrowAll(FHitResult targetHitResult, float addThrowImpulse);

	UFUNCTION(BlueprintImplementableEvent, Category = "Octopus Backpack | Parameters")
	void OctopusDamageActor(AActor *actor);

	void RegisterBackpack(class AOctopusBackpackActor* backpack);
	// Check trace block by this
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Octopus Backpack | Parameters")
	TArray<TEnumAsByte<ECollisionChannel>> objectDynamicTypesArr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Octopus Backpack | Parameters")
	UCurveFloat *walkMoveHandCurve = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Octopus Backpack | Parameters")
	UCurveFloat *walkMoveReturnHandCurve = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Octopus Backpack | Parameters")
	UCurveFloat *walkRotateHandCurve = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Octopus Backpack | Parameters")
	UCurveFloat *attackMoveHandCurve = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Octopus Backpack | Parameters")
	UCurveFloat *throwMoveHandCurve = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Octopus Backpack | Parameters")
	UCurveFloat *fallingMoveHandCurve = nullptr;
	
	UPROPERTY()
	FVector movementDirection = FVector::ZeroVector;
	UPROPERTY()
	FVector inputScale = FVector::ZeroVector;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Octopus Backpack | Parameters")
	TArray<FOctoHandStatus> handsArr;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Octopus Backpack | Parameters")
	TArray<AActor*> grabbedActorsArr;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Octopus Backpack | Parameters")
	bool bIsFlyingModeActivate = false;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Octopus Backpack | Parameters")
	bool bHasAnchorPoint = false;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Octopus Backpack | Parameters")
	bool bBattleMode = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Octopus Backpack | Parameters")
	bool bDevDebug = false;

	// UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Octopus Backpack | Debug")
	// bool bDebug = false;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Octopus Backpack | Parameters")
	class AOctopusBackpackActor* backPackOcto = nullptr;



protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	TArray<FHitResult> TraceBox(const FVector &startTrace, const FVector &endTrace, const FVector &boxSize) const;

	void InitializeOcto();

	void SetBooleanForHands();
	void SetHandToReturn(const int& handID);

	FTransform GetHandOriginTransform(const int &handID, const EOcto_HandPoint handPoint) const;
	FVector GetHandOriginLocationTraceCheck(const int &handID, const EOcto_HandPoint handPoint) const;
	// FRotator GetHandRotation(const int &handID) const;
	// FTransform GetHandGrabTransform(const int &handID) const;

	void GenerateIdleAnimateTransform(const int &handID);

	void ClearHandTime(const int &handID);
	void AttackHandTick(const int &handID, const float &deltaTime);
	void MoveToAttachPointTick(const int &handID, const float &deltaTime);
	void ReturnHandTick(const int &handID, const float &deltaTime);
	void IdleHandStateTick(const int &handID, const float &deltaTime);
	void GrabbedReturnTick(const int &handID, const float &deltaTime);
	void ThrowObjectTick(const int &handID, const float &deltaTime);
	void FallingHandsTick(const int &handID, const float &deltaTime);
	void SetFallingIdleHands();
	void HitBoxCalculate(TArray<FHitResult> &hitArr, TArray<int> &freeHandsArr, TArray<int> &idleHandsArr, EOcto_HandPoint handState);

private:

	UPROPERTY()
	FTimerHandle animateHandle;
	UPROPERTY()
	float animateRate = 0.5f;
	
	UPROPERTY()
	TArray<TEnumAsByte<EObjectTypeQuery>> traceObjectTypesArr;

	UPROPERTY()
	bool bFlyingDesiredState = false;
	
	UPROPERTY()
	bool bRegisterFromPlayer = false;
	UPROPERTY()
	FTimerHandle initializeHandle;
	UPROPERTY()
	float initializeTime = 0.25f;
	UPROPERTY()
	class ACharacter* myCharacter = nullptr;
	UPROPERTY()
	float searchDistance = 700.f;
	UPROPERTY()
	TArray<AActor*> actorsIgnoreArr;
	UPROPERTY()
	bool bRegistred = false;
	UPROPERTY()
	FVector boxRandAnim = FVector(200.f, 200.f, 150.f);
	UPROPERTY()
	FVector randWalkBoxSize = FVector(200.f, 200.f, 100.f);
	UPROPERTY()
	float throwDistanceHandMove = 1500.f;
	UPROPERTY()
	float attackDistanceHandMove = 3000.f;
	UPROPERTY()
	float moveDistance = 500.f;
	UPROPERTY()
	FVector checkBoxSize = FVector(800.f, 800.f, 1000.f);
	UPROPERTY()
	float maxHandDistance = 800.f;
	UPROPERTY()
	float maxHandDistanceSquared = 0.f;
	UPROPERTY()
	float breakDistance = 1000.f;
	UPROPERTY()
	float breakDistanceSquared = 0.f;
	UPROPERTY()
	float nextPointDistance = 500.f;
	UPROPERTY()
	float nextPointDistanceSquared = 0.f;
	UPROPERTY()
	bool bFallingHands = false;
	
		
};
