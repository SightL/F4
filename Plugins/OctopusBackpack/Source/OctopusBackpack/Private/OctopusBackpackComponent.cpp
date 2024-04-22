// Copyright 2024 CAS. All Rights Reserved.


#include "OctopusBackpackComponent.h"

#include "OctopusBackpackActor.h"
#include "Algo/RandomShuffle.h"
#include "Kismet/KismetMathLibrary.h"
#include "PhysicsEngine/BodySetup.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"

// Sets default values for this component's properties
UOctopusBackpackComponent::UOctopusBackpackComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UOctopusBackpackComponent::BeginPlay()
{
	Super::BeginPlay();

	maxHandDistanceSquared = FMath::Square(maxHandDistance);
	nextPointDistanceSquared = FMath::Square(nextPointDistance);
	breakDistanceSquared = FMath::Square(breakDistance);
}

TArray<FHitResult> UOctopusBackpackComponent::TraceBox(const FVector& startTrace, const FVector& endTrace, const FVector& boxSize) const
{
	FCollisionQueryParams traceParams_(TEXT("OctopusBackpack"), false);

	traceParams_.bReturnPhysicalMaterial = false;
	traceParams_.AddIgnoredActors(actorsIgnoreArr);

	const FQuat actorQuat_ = GetOwner()->GetActorQuat();

	TArray<FHitResult> hitArray_;

	GetOwner()->GetWorld()->SweepMultiByObjectType(hitArray_, startTrace, endTrace,
	                                               actorQuat_, traceObjectTypesArr,
	                                               FCollisionShape::MakeBox(boxSize), traceParams_);

	return hitArray_;
}

void UOctopusBackpackComponent::InitializeOcto()
{
	traceObjectTypesArr.Empty();
	handsArr.Empty();
	// Convert ECollisionChannel to ObjectType
	for (int i = 0; i < objectDynamicTypesArr.Num(); ++i)
	{
		traceObjectTypesArr.Add(UEngineTypes::ConvertToObjectType(objectDynamicTypesArr[i].GetValue()));
	}

	for (int i = 0; i < 4; ++i)
	{
		FOctoHandStatus newStatus_;
		handsArr.Add(newStatus_);
		handsArr[handsArr.Num() - 1].handTransform.SetLocation(backPackOcto->GetHandPointTransform(handsArr.Num() - 1, EOcto_HandPoint::FLY_ORIGIN).GetLocation());
		handsArr[handsArr.Num() - 1].handBeforeMove = handsArr[handsArr.Num() - 1].handTransform;
	}
}

void UOctopusBackpackComponent::SetBooleanForHands()
{
	bHasAnchorPoint = false;
	for (int i = 0; i < handsArr.Num(); ++i)
	{
		if (handsArr[i].action == EOctoHandAction::IDLE || handsArr[i].action == EOctoHandAction::FALLING) // || handsArr[i].action == EOctoHandAction::RETURNS)
		{
			handsArr[i].bBusyAlgo = false;
		}
		else
		{
			handsArr[i].bBusyAlgo = true;
		}

		if (handsArr[i].action == EOctoHandAction::ATTACHED)
		{
			bHasAnchorPoint = true;
		}
	}
}

void UOctopusBackpackComponent::SetHandToReturn(const int& handID)
{
	handsArr[handID].action = EOctoHandAction::RETURNS;
	handsArr[handID].handBeforeMove = handsArr[handID].handTransform;
	backPackOcto->ReturnEvent(backPackOcto->tentaclesArr[handID]);
	ClearHandTime(handID);
}

FTransform UOctopusBackpackComponent::GetHandOriginTransform(const int& handID, const EOcto_HandPoint handPoint) const
{
	return backPackOcto->GetHandPointTransform(handID, handPoint);
}

FVector UOctopusBackpackComponent::GetHandOriginLocationTraceCheck(const int& handID, const EOcto_HandPoint handPoint) const
{
	FCollisionQueryParams traceParams_(TEXT("OctopusBackpack"), false);
	traceParams_.bReturnPhysicalMaterial = false;
	traceParams_.AddIgnoredActors(actorsIgnoreArr);

	// Search Location for end Hand.
	FHitResult hitResult_;
	GetOwner()->GetWorld()->SweepSingleByObjectType(hitResult_, backPackOcto->GetActorLocation(), GetHandOriginTransform(handID, handPoint).GetLocation(),
	                                                GetOwner()->GetActorQuat(), traceObjectTypesArr,
	                                                FCollisionShape::MakeSphere(20.f), traceParams_);

	if (hitResult_.bBlockingHit)
	{
		return hitResult_.Location;
	}
	else
	{
		return GetHandOriginTransform(handID, handPoint).GetLocation();
	}
}

void UOctopusBackpackComponent::GenerateIdleAnimateTransform(const int& handID)
{
	handsArr[handID].bFinishAnimate = false;
	handsArr[handID].animTime = 0.f;

	bool bCanCameraMove_(false);

	float randPercent_ = FMath::RandRange(0.f, 100.f);
	if (randPercent_ > 90.f)
	{
		handsArr[handID].animRandRate = FMath::RandRange(10.f, 15.f);
		bCanCameraMove_ = true;
	}
	else if (randPercent_ > 70.f)
	{
		handsArr[handID].animRandRate = FMath::RandRange(5.f, 10.f);
		bCanCameraMove_ = true;
	}
	else if (randPercent_ > 30.f)
	{
		handsArr[handID].animRandRate = FMath::RandRange(0.5f, 5.f);
	}
	else if (randPercent_ >= 0.f)
	{
		handsArr[handID].animRandRate = FMath::RandRange(0.25f, 3.f);
	}

	if (FMath::RandRange(0, 100) > 70 && UGameplayStatics::GetTimeSeconds(GetWorld()) > 5.f && bCanCameraMove_)
	{
		FVector cameraLocation_ = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetCameraLocation();
		backPackOcto->tentaclesArr[handID].targetArrow->SetWorldLocation(UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetActorForwardVector() * FMath::RandRange(80.f, 200.f) + cameraLocation_);
		backPackOcto->tentaclesArr[handID].targetArrow->SetWorldRotation(UKismetMathLibrary::FindLookAtRotation(GetHandOriginTransform(handID, EOcto_HandPoint::FLY_ORIGIN).GetLocation(), cameraLocation_));
		backPackOcto->SetStretchLinearLimitsForTentacle(handID, true, EOctoHandAction::MOVE_TO_ATTACH_POINT);
		handsArr[handID].bCameraAnimation = true;
		backPackOcto->EnableIdleEffect(handID, false);
	}
	else if (UGameplayStatics::GetTimeSeconds(GetWorld()) > 5.f)
	{
		backPackOcto->tentaclesArr[handID].targetArrow->SetWorldLocation(UKismetMathLibrary::RandomPointInBoundingBox(GetHandOriginTransform(handID, EOcto_HandPoint::FLY_ORIGIN).GetLocation(), boxRandAnim));
		backPackOcto->tentaclesArr[handID].targetArrow->SetWorldRotation(UKismetMathLibrary::RandomRotator(true).Quaternion());
		backPackOcto->SetStretchLinearLimitsForTentacle(handID, false, EOctoHandAction::MOVE_TO_ATTACH_POINT);
		if (FMath::RandRange(0, 100) > 70)
		{
			backPackOcto->EnableIdleEffect(handID, FMath::RandBool());
		}
	}
}

void UOctopusBackpackComponent::ClearHandTime(const int& handID)
{
	handsArr[handID].walkMoveHandTime = 0.f;
	handsArr[handID].walkRotateHandTime = 0.f;
}

void UOctopusBackpackComponent::AttackHandTick(const int& handID, const float& deltaTime)
{
	float minCurveTime_;
	float maxCurveTime_;
	attackMoveHandCurve->GetTimeRange(minCurveTime_, maxCurveTime_);
	handsArr[handID].walkMoveHandTime += deltaTime;
	handsArr[handID].walkRotateHandTime += deltaTime;

	FVector interpolatedLocation_ = FMath::Lerp(handsArr[handID].handBeforeMove.GetLocation(), handsArr[handID].targetPoint.GetLocation(), attackMoveHandCurve->GetFloatValue(handsArr[handID].walkMoveHandTime));
	FQuat interpolatedRotation_ = FQuat::Slerp(handsArr[handID].handBeforeMove.GetRotation(), UKismetMathLibrary::FindLookAtRotation(backPackOcto->GetActorLocation(), handsArr[handID].targetPoint.GetLocation()).Quaternion(),
	                                           walkRotateHandCurve->GetFloatValue(handsArr[handID].walkRotateHandTime));
	handsArr[handID].handTransform.SetLocation(interpolatedLocation_);
	handsArr[handID].handTransform.SetRotation(interpolatedRotation_);

	FCollisionQueryParams traceParams_(TEXT("OctopusBackpack"), false);
	traceParams_.bReturnPhysicalMaterial = false;
	traceParams_.AddIgnoredActors(actorsIgnoreArr);
	// Search Location for end Hand.
	FHitResult hitResult_;
	FVector endLoc_ = backPackOcto->tentaclesArr[handID].handSkeletalComponent->GetForwardVector() * 25.f + handsArr[handID].handTransform.GetLocation();
	GetOwner()->GetWorld()->SweepSingleByChannel(hitResult_, backPackOcto->tentaclesArr[handID].handSkeletalComponent->GetComponentLocation(), endLoc_,
	                                             GetOwner()->GetActorQuat(), ECC_Visibility,
	                                             FCollisionShape::MakeSphere(25.f), traceParams_);

	if (handsArr[handID].walkMoveHandTime >= maxCurveTime_ || hitResult_.bBlockingHit)
	{
		if (IsValid(hitResult_.GetActor()))
		{
			OctopusDamageActor(hitResult_.GetActor());
		}

		bool bObjectGrabbed_(false);
		if (hitResult_.bBlockingHit)
		{
			if (hitResult_.Component->IsSimulatingPhysics() || hitResult_.Component->ComponentTags.Contains("octo") || hitResult_.GetActor()->Tags.Contains("octo"))
			{
				if (!IsComponentGrabbed(hitResult_.GetComponent()))
				{
					if (Cast<USkeletalMeshComponent>(hitResult_.GetComponent()))
					{
						USkeletalMeshComponent* skeletalMeshComponent_ = Cast<USkeletalMeshComponent>(hitResult_.GetComponent());

						skeletalMeshComponent_->SetAllBodiesSimulatePhysics(false);

						FName bone_ = "spine_01";
						skeletalMeshComponent_->SetAllBodiesBelowSimulatePhysics(bone_, true, true);
						skeletalMeshComponent_->SetAllBodiesBelowPhysicsBlendWeight(bone_, 1.0f, false, true);

						skeletalMeshComponent_->SetAllBodiesBelowSimulatePhysics("thigh_l", true, true);
						skeletalMeshComponent_->SetAllBodiesBelowPhysicsBlendWeight("thigh_l", 1.0f, false, true);

						skeletalMeshComponent_->SetAllBodiesBelowSimulatePhysics("thigh_r", true, true);
						skeletalMeshComponent_->SetAllBodiesBelowPhysicsBlendWeight("thigh_r", 1.0f, false, true);


						handsArr[handID].handTransform.SetLocation(hitResult_.ImpactPoint);
						backPackOcto->tentaclesArr[handID].handSkeletalComponent->SetWorldLocation(hitResult_.Location);
						FAttachmentTransformRules attachRule_(FAttachmentTransformRules::KeepWorldTransform);
						hitResult_.GetComponent()->AttachToComponent(backPackOcto->tentaclesArr[handID].handSkeletalComponent, attachRule_, "Arrow_home");
						handsArr[handID].action = EOctoHandAction::GRABBED_RETURN;
						handsArr[handID].handBeforeMove = handsArr[handID].handTransform;
						handsArr[handID].grabbedObject = hitResult_.GetComponent();
						bObjectGrabbed_ = true;
					}
					else
					{
						hitResult_.GetComponent()->SetSimulatePhysics(false);
						handsArr[handID].handTransform.SetLocation(hitResult_.ImpactPoint);
						backPackOcto->tentaclesArr[handID].handSkeletalComponent->SetWorldLocation(hitResult_.Location);
						FAttachmentTransformRules attachRule_(FAttachmentTransformRules::KeepWorldTransform);
						hitResult_.GetComponent()->AttachToComponent(backPackOcto->tentaclesArr[handID].handSkeletalComponent, attachRule_, "Arrow_home");
						handsArr[handID].action = EOctoHandAction::GRABBED_RETURN;
						handsArr[handID].handBeforeMove = handsArr[handID].handTransform;
						handsArr[handID].grabbedObject = hitResult_.GetComponent();
						bObjectGrabbed_ = true;
					}
					backPackOcto->ReturnEvent(backPackOcto->tentaclesArr[handID]);
				}
			}
		}

		if (!bObjectGrabbed_)
		{
			SetHandToReturn(handID);
		}

		ClearHandTime(handID);
	}
}

void UOctopusBackpackComponent::MoveToAttachPointTick(const int& handID, const float& deltaTime)
{
	float minCurveTime_;
	float maxCurveTime_;
	UCurveFloat* currentCurve_ = nullptr;
	if (bFallingHands)
	{
		currentCurve_ = fallingMoveHandCurve;
	}
	else
	{
		currentCurve_ = walkMoveHandCurve;
	}
	currentCurve_->GetTimeRange(minCurveTime_, maxCurveTime_);
	handsArr[handID].walkMoveHandTime += deltaTime;
	handsArr[handID].walkRotateHandTime += deltaTime;

	FVector interpolatedLocation_ = FMath::Lerp(handsArr[handID].handBeforeMove.GetLocation(), handsArr[handID].targetPoint.GetLocation(), currentCurve_->GetFloatValue(handsArr[handID].walkMoveHandTime));
	FQuat interpolatedRotation_ = FQuat::Slerp(handsArr[handID].handBeforeMove.GetRotation(), handsArr[handID].targetPoint.GetRotation(), walkRotateHandCurve->GetFloatValue(handsArr[handID].walkRotateHandTime));
	handsArr[handID].handTransform.SetLocation(interpolatedLocation_);
	handsArr[handID].handTransform.SetRotation(interpolatedRotation_);

	if (handsArr[handID].walkMoveHandTime >= maxCurveTime_)
	{
		handsArr[handID].action = EOctoHandAction::ATTACHED;
		ClearHandTime(handID);
		backPackOcto->SetStretchLinearLimitsForTentacle(handID, true, EOctoHandAction::MOVE_TO_ATTACH_POINT);
		bIsFlyingModeActivate = true;
		BroadcastOnOctopusFlyingMode(bIsFlyingModeActivate);
		backPackOcto->AttachedEvent(backPackOcto->tentaclesArr[handID]);

		backPackOcto->EnableIdleEffect(handID, false);

		for (int iHand_ = 0; iHand_ < handsArr.Num(); ++iHand_)
		{
			if (handID != iHand_ && handsArr[iHand_].action == EOctoHandAction::ATTACHED)
			{
				if (backPackOcto->tentaclesArr[iHand_].newYawRotation != 0.f)
				{
					backPackOcto->tentaclesArr[iHand_].newYawRotation = 0.f;
				}

				SetHandToReturn(iHand_);
			}
		}
	}
}

void UOctopusBackpackComponent::ReturnHandTick(const int& handID, const float& deltaTime)
{
	float minCurveTime_;
	float maxCurveTime_;
	walkMoveReturnHandCurve->GetTimeRange(minCurveTime_, maxCurveTime_);
	handsArr[handID].walkMoveHandTime += deltaTime;
	handsArr[handID].walkRotateHandTime += deltaTime;

	FVector interpolatedLocation_ = FMath::Lerp(handsArr[handID].handBeforeMove.GetLocation(), GetHandOriginTransform(handID, EOcto_HandPoint::FLY_ORIGIN).GetLocation(),
	                                            walkMoveReturnHandCurve->GetFloatValue(handsArr[handID].walkMoveHandTime));
	FQuat interpolatedRotation_ = FQuat::Slerp(handsArr[handID].handBeforeMove.GetRotation(), FRotator::ZeroRotator.Quaternion(), walkRotateHandCurve->GetFloatValue(handsArr[handID].walkRotateHandTime));
	handsArr[handID].handTransform.SetLocation(interpolatedLocation_);
	handsArr[handID].handTransform.SetRotation(interpolatedRotation_);

	if (handsArr[handID].walkMoveHandTime >= maxCurveTime_)
	{
		handsArr[handID].action = EOctoHandAction::IDLE;
		backPackOcto->SetStretchLinearLimitsForTentacle(handID, false);
		ClearHandTime(handID);
	}
}

void UOctopusBackpackComponent::IdleHandStateTick(const int& handID, const float& deltaTime)
{
	if (inputScale == FVector::ZeroVector && handsArr[handID].animIdleTime <= 0.f)
	{
		if (handsArr[handID].bFinishAnimate)
		{
			GenerateIdleAnimateTransform(handID);
		}

		FCollisionQueryParams traceParams_(TEXT("OctopusBackpack"), false);
		traceParams_.bReturnPhysicalMaterial = false;
		traceParams_.AddIgnoredActors(actorsIgnoreArr);

		// Search Location for end Hand.
		FHitResult outHit_;
		GetOwner()->GetWorld()->SweepSingleByObjectType(outHit_, backPackOcto->GetActorLocation(), backPackOcto->tentaclesArr[handID].targetArrow->GetComponentLocation(),
		                                                GetOwner()->GetActorQuat(), traceObjectTypesArr,
		                                                FCollisionShape::MakeSphere(20.f), traceParams_);
		FTransform animationTransform_;
		if (outHit_.bBlockingHit)
		{
			animationTransform_.SetLocation(outHit_.Location);
		}
		else
		{
			animationTransform_.SetLocation(backPackOcto->tentaclesArr[handID].targetArrow->GetComponentTransform().GetLocation());
		}

		if (handsArr[handID].bCameraAnimation)
		{
			animationTransform_.SetRotation(UKismetMathLibrary::FindLookAtRotation(handsArr[handID].handTransform.GetLocation(), UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetCameraLocation()).Quaternion());
		}
		else
		{
			animationTransform_.SetRotation(backPackOcto->tentaclesArr[handID].targetArrow->GetComponentTransform().GetRotation());
		}

		handsArr[handID].handTransform = UKismetMathLibrary::TInterpTo(handsArr[handID].handTransform, animationTransform_, deltaTime, handsArr[handID].animRandRate);
		handsArr[handID].animTime += deltaTime;

		if (handsArr[handID].handTransform.GetLocation().Equals(animationTransform_.GetLocation(), 5.0f) || handsArr[handID].animTime >= 5.f)
		{
			handsArr[handID].bFinishAnimate = true;
			handsArr[handID].bCameraAnimation = false;
		}
	}
	else
	{
		// Set idle time after moving before animation.
		if (handsArr[handID].animIdleTime <= 0.f)
		{
			handsArr[handID].animIdleTime = FMath::RandRange(1.f, 7.f);
		}

		if (inputScale == FVector::ZeroVector)
		{
			handsArr[handID].animIdleTime = FMath::Max(handsArr[handID].animIdleTime - deltaTime, 0.f);
		}

		if (handsArr[handID].bCameraAnimation)
		{
			backPackOcto->SetStretchLinearLimitsForTentacle(handID, false, EOctoHandAction::MOVE_TO_ATTACH_POINT);
			handsArr[handID].bCameraAnimation = false;
			handsArr[handID].bFinishAnimate = true;
		}

		FTransform originHandTransform_;
		if (bIsFlyingModeActivate)
		{
			originHandTransform_.SetLocation(GetHandOriginLocationTraceCheck(handID, EOcto_HandPoint::FLY_ORIGIN));
			originHandTransform_.SetRotation(GetHandOriginTransform(handID, EOcto_HandPoint::FLY_ORIGIN).GetRotation());
		}
		else
		{
			originHandTransform_.SetLocation(GetHandOriginLocationTraceCheck(handID, EOcto_HandPoint::WALK_ORIGIN));
			originHandTransform_.SetRotation(GetHandOriginTransform(handID, EOcto_HandPoint::WALK_ORIGIN).GetRotation());
		}

		if (handsArr[handID].handTransform.GetLocation().Equals(originHandTransform_.GetLocation(), 5.0f))
		{
			handsArr[handID].handTransform = originHandTransform_;
		}
		else
		{
			handsArr[handID].handTransform = UKismetMathLibrary::TInterpTo(handsArr[handID].handTransform, originHandTransform_, deltaTime, 15.f);
		}
	}
}

void UOctopusBackpackComponent::GrabbedReturnTick(const int& handID, const float& deltaTime)
{
	float minCurveTime_;
	float maxCurveTime_;
	walkMoveReturnHandCurve->GetTimeRange(minCurveTime_, maxCurveTime_);
	handsArr[handID].walkMoveHandTime += deltaTime;
	handsArr[handID].walkRotateHandTime += deltaTime;

	FVector interpolatedLocation_ = FMath::Lerp(handsArr[handID].handBeforeMove.GetLocation(), GetHandOriginTransform(handID, EOcto_HandPoint::FLY_GRAB_ORIGIN).GetLocation(),
	                                            walkMoveReturnHandCurve->GetFloatValue(handsArr[handID].walkMoveHandTime));
	FQuat interpolatedRotation_ = FQuat::Slerp(handsArr[handID].handBeforeMove.GetRotation(), GetHandOriginTransform(handID, EOcto_HandPoint::FLY_GRAB_ORIGIN).Rotator().Quaternion(),
	                                           walkRotateHandCurve->GetFloatValue(handsArr[handID].walkRotateHandTime));
	handsArr[handID].handTransform.SetLocation(interpolatedLocation_);
	handsArr[handID].handTransform.SetRotation(interpolatedRotation_);

	if (handsArr[handID].walkMoveHandTime >= maxCurveTime_)
	{
		backPackOcto->SetStretchLinearLimitsForTentacle(handID, false);
		handsArr[handID].action = EOctoHandAction::GRABBED;
		ClearHandTime(handID);
	}
}

void UOctopusBackpackComponent::ThrowObjectTick(const int& handID, const float& deltaTime)
{
	float minCurveTime_;
	float maxCurveTime_;
	throwMoveHandCurve->GetTimeRange(minCurveTime_, maxCurveTime_);
	handsArr[handID].walkMoveHandTime += deltaTime;
	handsArr[handID].walkRotateHandTime += deltaTime;

	FVector interpolatedLocation_ = FMath::Lerp(handsArr[handID].handBeforeMove.GetLocation(), handsArr[handID].targetPoint.GetLocation(), throwMoveHandCurve->GetFloatValue(handsArr[handID].walkMoveHandTime));
	FQuat interpolatedRotation_ = FQuat::Slerp(handsArr[handID].handBeforeMove.GetRotation(), handsArr[handID].targetPoint.GetRotation(), walkRotateHandCurve->GetFloatValue(handsArr[handID].walkRotateHandTime));
	handsArr[handID].handTransform.SetLocation(interpolatedLocation_);
	handsArr[handID].handTransform.SetRotation(interpolatedRotation_);

	FCollisionQueryParams traceParams_(TEXT("OctopusBackpack"), false);
	traceParams_.bReturnPhysicalMaterial = false;
	traceParams_.AddIgnoredActors(actorsIgnoreArr);

	UPrimitiveComponent* primitiveComponent_ = Cast<UPrimitiveComponent>(handsArr[handID].grabbedObject);
	if (primitiveComponent_)
	{
		traceParams_.AddIgnoredComponent(primitiveComponent_);

		// Search Location for end Hand.
		FHitResult hitResult_;
		float bounds_ = primitiveComponent_->Bounds.BoxExtent.GetMax();
		float traceDistance_ = bounds_ * 1.3f;
		FVector endLoc_ = UKismetMathLibrary::FindLookAtRotation(handsArr[handID].handTransform.GetLocation(), handsArr[handID].targetPoint.GetLocation()).Vector() * traceDistance_ + handsArr[handID].handTransform.GetLocation();

		GetOwner()->GetWorld()->SweepSingleByChannel(hitResult_, handsArr[handID].handTransform.GetLocation(), endLoc_,
		                                             GetOwner()->GetActorQuat(), ECC_Visibility, FCollisionShape::MakeSphere(bounds_), traceParams_);

		if (handsArr[handID].walkMoveHandTime >= maxCurveTime_ || hitResult_.bBlockingHit)
		{
			FDetachmentTransformRules detachRule_(FDetachmentTransformRules::KeepWorldTransform);
			primitiveComponent_->DetachFromComponent(detachRule_);
			primitiveComponent_->SetSimulatePhysics(true);
			primitiveComponent_->AddImpulse(handsArr[handID].handTransform.GetRotation().GetForwardVector() * handsArr[handID].throwImpulse);

			SetHandToReturn(handID);
			handsArr[handID].grabbedObject = nullptr;
		}
	}
}

void UOctopusBackpackComponent::FallingHandsTick(const int& handID, const float& deltaTime)
{
	float minCurveTime_;
	float maxCurveTime_;
	walkMoveReturnHandCurve->GetTimeRange(minCurveTime_, maxCurveTime_);
	handsArr[handID].walkMoveHandTime += deltaTime;
	handsArr[handID].walkRotateHandTime += deltaTime;

	FVector interpolatedLocation_ = FMath::Lerp(handsArr[handID].handBeforeMove.GetLocation(), GetHandOriginLocationTraceCheck(handID, EOcto_HandPoint::FALLING_ORIGIN),
	                                            walkMoveReturnHandCurve->GetFloatValue(handsArr[handID].walkMoveHandTime));
	FQuat interpolatedRotation_ = FQuat::Slerp(handsArr[handID].handBeforeMove.GetRotation(), GetHandOriginTransform(handID, EOcto_HandPoint::FALLING_ORIGIN).Rotator().Quaternion(),
	                                           walkRotateHandCurve->GetFloatValue(handsArr[handID].walkRotateHandTime));
	handsArr[handID].handTransform.SetLocation(interpolatedLocation_);
	handsArr[handID].handTransform.SetRotation(interpolatedRotation_);

	if (handsArr[handID].walkMoveHandTime >= maxCurveTime_)
	{
		backPackOcto->SetStretchLinearLimitsForTentacle(handID, false);

		APawn* myPawn_ = Cast<APawn>(GetOwner());
		if (myPawn_)
		{
			if (!myPawn_->GetMovementComponent()->IsFalling())
			{
				SetHandToReturn(handID);
			}
		}
	}
}

void UOctopusBackpackComponent::SetFallingIdleHands()
{
	for (int i = 0; i < handsArr.Num(); ++i)
	{
		if (handsArr[i].action == EOctoHandAction::IDLE ||
			handsArr[i].action == EOctoHandAction::RETURNS)
		{
			handsArr[i].action = EOctoHandAction::FALLING;
			handsArr[i].handBeforeMove = handsArr[i].handTransform;
			ClearHandTime(i);
		}
	}
}

void UOctopusBackpackComponent::HitBoxCalculate(TArray<FHitResult>& hitArr, TArray<int>& freeHandsArr, TArray<int>& idleHandsArr, EOcto_HandPoint handState)
{
	EOcto_HandPoint handState_(EOcto_HandPoint::FLY_ORIGIN);
	if (handState == EOcto_HandPoint::FALLING_ORIGIN)
	{
		handState_ = EOcto_HandPoint::FALLING_ORIGIN;
	}
	for (int i = hitArr.Num() - 1; i >= 0; --i)
	{
		if (hitArr[i].bBlockingHit)
		{
			if (hitArr[i].GetActor()->IsA(AOctopusBackpackActor::StaticClass()))
			{
				continue;
			}

			UPrimitiveComponent* primComp_(Cast<UPrimitiveComponent>(hitArr[i].GetComponent()));
			if (primComp_)
			{
				// Get collision complexity from body setup
				UBodySetup* bodySetup_ = primComp_->GetBodySetup();
				if (bodySetup_)
				{
					if (bodySetup_->CollisionTraceFlag == CTF_UseSimpleAsComplex || bodySetup_->CollisionTraceFlag == CTF_UseDefault)
					{
						for (int iClose_ = 0; iClose_ < handsArr.Num(); ++iClose_)
						{
							if (!handsArr[iClose_].bBusyAlgo)
							{
								FVector closestPoint_;
								FVector newPoint_ = UKismetMathLibrary::RandomPointInBoundingBox(GetHandOriginTransform(iClose_, handState_).GetLocation(), randWalkBoxSize);
								primComp_->GetClosestPointOnCollision(newPoint_ + movementDirection * searchDistance, closestPoint_);

								if ((closestPoint_ - GetHandOriginTransform(iClose_, handState_).GetLocation()).SizeSquared() < breakDistanceSquared)
								{
									freeHandsArr.AddUnique(iClose_);
									handsArr[iClose_].targetPointsArr.Add(closestPoint_);

									if (handsArr[iClose_].action == EOctoHandAction::IDLE || handsArr[iClose_].action == EOctoHandAction::FALLING)
									{
										idleHandsArr.AddUnique(iClose_);
									}

									if (bDevDebug)
									{
										DrawDebugSphere(GetWorld(), closestPoint_, 20.0f, 8, FColor::Yellow, false, 0.3f);
									}
								}

								// FVector closestPointSecond_;
								//
								// FVector newPointActor_ = UKismetMathLibrary::RandomPointInBoundingBox(GetOwner()->GetActorLocation(), randBoxSize_);
								// newPointActor_ = GetOwner()->GetActorLocation();
								// primComp_->GetClosestPointOnCollision(newPointActor_, closestPointSecond_);
								//
								// if ((closestPointSecond_ - GetOwner()->GetActorLocation()).SizeSquared() < breakDistanceSquared)
								// {
								// 	freeHandsArr.AddUnique(iClose_);
								// 	handsArr[iClose_].targetPointsArr.Add(closestPointSecond_);
								//
								// 	if (handsArr[iClose_].action == EOctoHandAction::IDLE || handsArr[iClose_].action == EOctoHandAction::FALLING)
								// 	{
								// 		idleHandsArr.AddUnique(iClose_);
								// 	}
								//
								// 	if (bDevDebug)
								// 	{
								// 		DrawDebugSphere(GetWorld(), closestPointSecond_, 20.0f, 8, FColor::Purple, false, 0.3f);
								// 	}
								// }
								if (bDevDebug)
								{
									DrawDebugSphere(GetWorld(), GetHandOriginTransform(iClose_, handState_).GetLocation() + movementDirection * searchDistance, 20.0f, 8, FColor::Orange, false, 0.3f);
								}
							}
						}
					}
				}
			}
		}
	}
}

// Called every frame
void UOctopusBackpackComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bRegistred || !bRegisterFromPlayer || !bBattleMode)
	{
		return;
	}

	if (!IsValid(backPackOcto) || !walkMoveHandCurve || !walkRotateHandCurve || !walkMoveReturnHandCurve || !throwMoveHandCurve || !attackMoveHandCurve)
	{
		if (!IsValid(backPackOcto))
		{
			GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Red, TEXT("ERROR - nullptr reference"));
			return;
		}
		GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Red, TEXT("ERROR - nullptr reference"));
		return;
	}

	for (int i = 0; i < handsArr.Num(); ++i)
	{
		if (!handsArr[i].targetPointsArr.IsEmpty())
		{
			handsArr[i].targetPointsArr.Empty();
		}
	}

	SetBooleanForHands();

	FVector checkBoxLocation_ = backPackOcto->GetActorLocation();
	checkBoxLocation_ = movementDirection * moveDistance + checkBoxLocation_;
	TArray<FHitResult> boxesHitFallingArr_;
	bool bHandMoved_(false);
	for (int i = 0; i < handsArr.Num(); ++i)
	{
		if (handsArr[i].action == EOctoHandAction::MOVE_TO_ATTACH_POINT)
		{
			bHandMoved_ = true;
			MoveToAttachPointTick(i, DeltaTime);
		}
		else if (handsArr[i].action == EOctoHandAction::RETURNS)
		{
			ReturnHandTick(i, DeltaTime);
		}
		else if (handsArr[i].action == EOctoHandAction::IDLE)
		{
			IdleHandStateTick(i, DeltaTime);
		}
		else if (handsArr[i].action == EOctoHandAction::ATTACKS)
		{
			bHandMoved_ = true;
			AttackHandTick(i, DeltaTime);
		}
		else if (handsArr[i].action == EOctoHandAction::GRABBED_RETURN)
		{
			GrabbedReturnTick(i, DeltaTime);
		}
		else if (handsArr[i].action == EOctoHandAction::GRABBED)
		{
			if (bIsFlyingModeActivate)
			{
				handsArr[i].handTransform.SetLocation(GetHandOriginLocationTraceCheck(i, EOcto_HandPoint::FLY_GRAB_ORIGIN));
				handsArr[i].handTransform.SetRotation(GetHandOriginTransform(i, EOcto_HandPoint::FLY_GRAB_ORIGIN).Rotator().Quaternion());
			}
			else
			{
				handsArr[i].handTransform.SetLocation(GetHandOriginLocationTraceCheck(i, EOcto_HandPoint::WALK_GRAB_ORIGIN));
				handsArr[i].handTransform.SetRotation(GetHandOriginTransform(i, EOcto_HandPoint::WALK_GRAB_ORIGIN).Rotator().Quaternion());
			}
		}
		else if (handsArr[i].action == EOctoHandAction::THROW)
		{
			ThrowObjectTick(i, DeltaTime);
		}
		else if (handsArr[i].action == EOctoHandAction::FALLING)
		{
			FallingHandsTick(i, DeltaTime);
			float boxSideSize_ = 100.f;
			TArray<FHitResult> tempHitArr_ = TraceBox(handsArr[i].handTransform.GetLocation(), handsArr[i].handTransform.GetLocation(), FVector(boxSideSize_, boxSideSize_, boxSideSize_));
			boxesHitFallingArr_.Append(tempHitArr_);
		}
	}


	FVector currentAttachPoint_(FVector::ZeroVector);

	if (!bHandMoved_)
	{
		bool bNeedNewPoint_ = false;
		bool bHasHandAttached_ = false;
		bool bBreakHand_ = false;
		for (int i = 0; i < handsArr.Num(); ++i)
		{
			if (handsArr[i].action == EOctoHandAction::ATTACHED)
			{
				bHasHandAttached_ = true;
				currentAttachPoint_ = handsArr[i].targetPoint.GetLocation();
				if (inputScale != FVector::ZeroVector)
				{
					if ((handsArr[i].targetPoint.GetLocation() - GetHandOriginTransform(i, EOcto_HandPoint::FLY_ORIGIN).GetLocation()).SizeSquared() > nextPointDistanceSquared)
					{
						bNeedNewPoint_ = true;
					}
				}
				if ((handsArr[i].targetPoint.GetLocation() - GetHandOriginTransform(i, EOcto_HandPoint::FLY_ORIGIN).GetLocation()).SizeSquared() > breakDistanceSquared)
				{
					bNeedNewPoint_ = true;
					bBreakHand_ = true;
					SetHandToReturn(i);
					bIsFlyingModeActivate = false;
					BroadcastOnOctopusFlyingMode(bIsFlyingModeActivate);
				}
			}
		}

		TArray<int> freeHandsIndexArr_;
		TArray<int> idleHandsIndexArr_;
		bool bFalling_(false);
		bFallingHands = false;

		if (bFlyingDesiredState)
		{
			if (!bHasHandAttached_ || bNeedNewPoint_)
			{
				if (bBreakHand_ || !bHasHandAttached_)
				{
					APawn* myPawn_ = Cast<APawn>(GetOwner());
					if (myPawn_)
					{
						if (myPawn_->GetMovementComponent()->IsFalling())
						{
							SetFallingIdleHands();
							bFalling_ = true;
							bFallingHands = true;
						}
					}
				}

				TArray<FHitResult> boxHitResultsArr_ = TraceBox(checkBoxLocation_, checkBoxLocation_, checkBoxSize);
				HitBoxCalculate(boxHitResultsArr_, freeHandsIndexArr_, idleHandsIndexArr_, EOcto_HandPoint::FLY_ORIGIN);
				if (bFalling_)
				{
					HitBoxCalculate(boxesHitFallingArr_, freeHandsIndexArr_, idleHandsIndexArr_, EOcto_HandPoint::FALLING_ORIGIN);
				}
			}
		}

		if (bDevDebug)
		{
			DrawDebugBox(GetOwner()->GetWorld(), checkBoxLocation_, checkBoxSize, GetOwner()->GetActorQuat(), FColor::Yellow, false, 0.f, 0, 2.f);
		}

		if (bFlyingDesiredState)
		{
			// Activate Random Hand.
			if (freeHandsIndexArr_.Num() > 0)
			{
				TArray<int> useHandArr_;
				if (idleHandsIndexArr_.Num() > 0)
				{
					useHandArr_ = idleHandsIndexArr_;
				}
				else
				{
					useHandArr_ = freeHandsIndexArr_;
				}

				if (useHandArr_.Num() > 1)
				{
					Algo::RandomShuffle(useHandArr_);
				}

				float minSizeFirst_(MAX_flt);
				float minSizeSecond_(MAX_flt);
				FVector targetFirst_;
				FVector targetSecond_;
				for (int iPoint_ = 0; iPoint_ < handsArr[useHandArr_[0]].targetPointsArr.Num(); ++iPoint_)
				{
					if (!currentAttachPoint_.Equals(handsArr[useHandArr_[0]].targetPointsArr[iPoint_], 30.f))
					{
						float checkSize_ = (handsArr[useHandArr_[0]].targetPointsArr[iPoint_] - (backPackOcto->GetActorLocation() + movementDirection * searchDistance)).SizeSquared();
						if (checkSize_ < minSizeFirst_)
						{
							minSizeSecond_ = minSizeFirst_;
							minSizeFirst_ = checkSize_;
							targetSecond_ = targetFirst_;
							targetFirst_ = handsArr[useHandArr_[0]].targetPointsArr[iPoint_];
						}
						else if (checkSize_ < minSizeSecond_)
						{
							minSizeSecond_ = checkSize_;
							targetSecond_ = handsArr[useHandArr_[0]].targetPointsArr[iPoint_];
						}
					}
				}

				TArray<FVector> targetPointsArr_;
				targetPointsArr_.Add(targetFirst_);
				if (minSizeSecond_ != MAX_flt)
				{
					if (minSizeSecond_ < maxHandDistanceSquared)
					{
						targetPointsArr_.Add(targetSecond_);
					}
				}
				if (minSizeFirst_ != MAX_flt)
				{
					if (targetPointsArr_.Num() > 1)
					{
						Algo::RandomShuffle(targetPointsArr_);
					}

					FCollisionQueryParams traceParams_(TEXT("OctopusBackpack"), false);
					traceParams_.bReturnPhysicalMaterial = false;
					traceParams_.AddIgnoredActors(actorsIgnoreArr);

					// Search Location for end Hand.
					TArray<FHitResult> hitResultArr_;
					FVector endLoc_ = UKismetMathLibrary::FindLookAtRotation(backPackOcto->GetActorLocation(), targetPointsArr_[0]).Vector() * ((backPackOcto->GetActorLocation() - targetPointsArr_[0]).Size() + 25.f) + backPackOcto->
						GetActorLocation();

					GetOwner()->GetWorld()->SweepMultiByObjectType(hitResultArr_, backPackOcto->GetActorLocation(), endLoc_,
					                                               GetOwner()->GetActorQuat(), traceObjectTypesArr,
					                                               FCollisionShape::MakeSphere(10.f), traceParams_);

					for (int iHit_ = 0; iHit_ < hitResultArr_.Num(); ++iHit_)
					{
						if (hitResultArr_[iHit_].bBlockingHit)
						{
							if (hitResultArr_[iHit_].GetActor() != backPackOcto)
							{
								FRotator handRotation_ = UKismetMathLibrary::FindLookAtRotation(hitResultArr_[iHit_].Location, hitResultArr_[iHit_].ImpactPoint); //FRotationMatrix::MakeFromX(-hitResultArr_[iHit_].ImpactNormal).Rotator();

								handsArr[useHandArr_[0]].targetPoint.SetLocation(handRotation_.Vector() * -30.f + hitResultArr_[iHit_].ImpactPoint); //(hitResultArr_[iHit_].Location);
								handsArr[useHandArr_[0]].targetHitResult = hitResultArr_[iHit_];
								handsArr[useHandArr_[0]].bBusyAlgo = true;
								handsArr[useHandArr_[0]].action = EOctoHandAction::MOVE_TO_ATTACH_POINT;
								backPackOcto->MoveToAttachPointEvent(backPackOcto->tentaclesArr[useHandArr_[0]]);
								handsArr[useHandArr_[0]].handBeforeMove = handsArr[useHandArr_[0]].handTransform;


								handsArr[useHandArr_[0]].targetPoint.SetRotation(handRotation_.Quaternion());
								break;
							}
						}
					}
				}
			}
		}
	}

	if (bDevDebug)
	{
		DrawDebugSphere(GetWorld(), movementDirection * moveDistance + backPackOcto->GetActorLocation(), 25.0f, 8, FColor::Blue, false, 0.f, 0, 5.f);
		for (int i = 0; i < handsArr.Num(); ++i)
		{
			if (handsArr[i].bBusyAlgo)
			{
				DrawDebugSphere(GetWorld(), handsArr[i].targetPoint.GetLocation(), 25.0f, 8, FColor::Green, false, 0.f, 0, 1.f);
			}
		}
	}

	bool bCanMove_ = false;
	for (int i = 0; i < handsArr.Num(); ++i)
	{
		if (handsArr[i].action == EOctoHandAction::ATTACHED)
		{
			if ((handsArr[i].targetPoint.GetLocation() - backPackOcto->GetActorLocation()).SizeSquared() >= maxHandDistanceSquared)
			{
				bCanMove_ = false;
				break;
			}
			bCanMove_ = true;
		}
	}

	if (bCanMove_)
	{
		BroadcastOnOctopusReturnMovement(inputScale);
	}
	else
	{
		//	FVector targetPosition_;
		FVector characterMovementVector_ = movementDirection;
		for (int i = 0; i < handsArr.Num(); ++i)
		{
			if (handsArr[i].action == EOctoHandAction::ATTACHED)
			{
				FVector toTarget_ = handsArr[i].targetPoint.GetLocation() - GetOwner()->GetActorLocation();
				toTarget_.Normalize();
				float dotProduct = FVector::DotProduct(toTarget_, characterMovementVector_.GetSafeNormal(0.0001f));
				if (dotProduct > 0)
				{
					BroadcastOnOctopusReturnMovement(inputScale);
					break;
				}
			}
		}
	}

	movementDirection = FVector::ZeroVector;
	inputScale = FVector::ZeroVector;
}

void UOctopusBackpackComponent::BroadcastOnOctopusReturnMovement(const FVector& inScale) const
{
	OnOctopusReturnMovement.Broadcast(inScale);
}

void UOctopusBackpackComponent::BroadcastOnOctopusFlyingMode(const bool& bIsActivated) const
{
	OnOctopusFlyingMode.Broadcast(bIsActivated);
}

void UOctopusBackpackComponent::OctopusBackpackFlyingMode(bool bActivate)
{
	if (bBattleMode && backPackOcto->bIsHandsShowed)
	{
		bFlyingDesiredState = bActivate;
		if (!bFlyingDesiredState)
		{
			bIsFlyingModeActivate = false;
			BroadcastOnOctopusFlyingMode(bIsFlyingModeActivate);

			for (int i = 0; i < handsArr.Num(); ++i)
			{
				if (handsArr[i].action == EOctoHandAction::ATTACHED || handsArr[i].action == EOctoHandAction::MOVE_TO_ATTACH_POINT)
				{
					SetHandToReturn(i);
				}
			}
		}
	}
}

void UOctopusBackpackComponent::OctopusBackpackBattleMode(bool bActivate)
{
	if (!bActivate)
	{
		if (IsValid(backPackOcto))
		{
			if (backPackOcto->bIsHandsShowed)
			{
				DropAll();
				OctopusBackpackFlyingMode(false);
				bBattleMode = false;
				backPackOcto->ActivateBackpack(false);
			}
		}
	}
	else
	{
		if (IsValid(backPackOcto))
		{
			bBattleMode = true;
			backPackOcto->ActivateBackpack(bBattleMode);
			for (int i = 0; i < handsArr.Num(); ++i)
			{
				handsArr[i].action = EOctoHandAction::IDLE;
				handsArr[i].handBeforeMove = GetHandOriginTransform(i, EOcto_HandPoint::WALK_ORIGIN);
				handsArr[i].handTransform = handsArr[i].handBeforeMove;
				ClearHandTime(i);
			}
		}
	}
}

void UOctopusBackpackComponent::RegisterOctopusBackpack()
{
	bRegisterFromPlayer = true;

	if (!actorsIgnoreArr.IsEmpty())
	{
		actorsIgnoreArr.Empty();
	}

	actorsIgnoreArr.Add(GetOwner());
	if (backPackOcto)
	{
		actorsIgnoreArr.Add(backPackOcto);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("ERROR backPackOcto reference equal nullptr"));
	}
}

void UOctopusBackpackComponent::Attack(FHitResult targetHitResult)
{
	if (bBattleMode && bRegistred)
	{
		TArray<int> handIndexArr_;
		for (int i = 0; i < handsArr.Num(); ++i)
		{
			if (handsArr[i].action == EOctoHandAction::IDLE || handsArr[i].action == EOctoHandAction::RETURNS)
			{
				handIndexArr_.Add(i);
			}
		}

		if (handIndexArr_.Num() > 0)
		{
			int handIndex_(handIndexArr_[0]);
			if (handIndexArr_.Num() > 1)
			{
				handIndex_ = handIndexArr_[FMath::RandRange(0, handIndexArr_.Num() - 1)];
			}

			if (handIndexArr_.Num() > 0)
			{
				if (targetHitResult.bBlockingHit)
				{
					FRotator handRotation_ = UKismetMathLibrary::FindLookAtRotation(handsArr[handIndex_].handTransform.GetLocation(), targetHitResult.ImpactPoint);
					handsArr[handIndex_].targetPoint.SetLocation(handRotation_.Vector() * attackDistanceHandMove + handsArr[handIndex_].handTransform.GetLocation());
				}
				else
				{
					FRotator handRotation_ = UKismetMathLibrary::FindLookAtRotation(backPackOcto->GetActorLocation(), targetHitResult.TraceEnd);
					handsArr[handIndex_].targetPoint.SetLocation(handRotation_.Vector() * attackDistanceHandMove + handsArr[handIndex_].handTransform.GetLocation());
				}
				backPackOcto->EnableIdleEffect(handIndex_, false);
				handsArr[handIndex_].action = EOctoHandAction::ATTACKS;
				backPackOcto->SetStretchLinearLimitsForTentacle(handIndex_, true, EOctoHandAction::ATTACKS);
				handsArr[handIndex_].handBeforeMove = handsArr[handIndex_].handTransform;
				handsArr[handIndex_].animIdleTime = FMath::RandRange(2.f, 7.f);
				backPackOcto->AttackMoveEvent(backPackOcto->tentaclesArr[handIndex_]);
			}
		}
	}
}

void UOctopusBackpackComponent::AttackHand(FHitResult targetHitResult, int handIndex)
{
	if (bBattleMode && bRegistred)
	{
		if (handsArr.IsValidIndex(handIndex))
		{
			if (handsArr[handIndex].action == EOctoHandAction::IDLE || handsArr[handIndex].action == EOctoHandAction::RETURNS)
			{
				if (targetHitResult.bBlockingHit)
				{
					FRotator handRotation_ = UKismetMathLibrary::FindLookAtRotation(handsArr[handIndex].handTransform.GetLocation(), targetHitResult.ImpactPoint);
					handsArr[handIndex].targetPoint.SetLocation(handRotation_.Vector() * attackDistanceHandMove + handsArr[handIndex].handTransform.GetLocation());
				}
				else
				{
					FRotator handRotation_ = UKismetMathLibrary::FindLookAtRotation(handsArr[handIndex].handTransform.GetLocation(), targetHitResult.TraceEnd);
					handsArr[handIndex].targetPoint.SetLocation(handRotation_.Vector() * attackDistanceHandMove + handsArr[handIndex].handTransform.GetLocation());
				}
				backPackOcto->EnableIdleEffect(handIndex, false);
				handsArr[handIndex].action = EOctoHandAction::ATTACKS;
				backPackOcto->SetStretchLinearLimitsForTentacle(handIndex, true, EOctoHandAction::ATTACKS);
				handsArr[handIndex].handBeforeMove = handsArr[handIndex].handTransform;
				handsArr[handIndex].animIdleTime = FMath::RandRange(2.f, 7.f);
				backPackOcto->AttackMoveEvent(backPackOcto->tentaclesArr[handIndex]);
			}
		}
	}
}

void UOctopusBackpackComponent::ThrowAll(FHitResult targetHitResult, float addThrowImpulse)
{
	if (IsValid(backPackOcto) && bBattleMode && bRegistred)
	{
		for (int i = 0; i < handsArr.Num(); ++i)
		{
			if (handsArr[i].action == EOctoHandAction::GRABBED_RETURN || handsArr[i].action == EOctoHandAction::GRABBED)
			{
				handsArr[i].action = EOctoHandAction::THROW;
				handsArr[i].handBeforeMove = handsArr[i].handTransform;
				handsArr[i].throwImpulse = addThrowImpulse;
				backPackOcto->SetStretchLinearLimitsForTentacle(i, true, EOctoHandAction::ATTACKS);

				if (targetHitResult.bBlockingHit)
				{
					FRotator targetRot_ = UKismetMathLibrary::FindLookAtRotation(handsArr[i].handTransform.GetLocation(), targetHitResult.ImpactPoint);
					handsArr[i].targetPoint.SetLocation(targetRot_.Vector() * throwDistanceHandMove + handsArr[i].handTransform.GetLocation());
					handsArr[i].targetPoint.SetRotation(targetRot_.Quaternion());
				}
				else
				{
					FRotator targetRot_ = UKismetMathLibrary::FindLookAtRotation(handsArr[i].handTransform.GetLocation(), targetHitResult.TraceEnd);
					handsArr[i].targetPoint.SetLocation(targetRot_.Vector() * throwDistanceHandMove + handsArr[i].handTransform.GetLocation());
					handsArr[i].targetPoint.SetRotation(targetRot_.Quaternion());
				}
			}
		}
	}
}

bool UOctopusBackpackComponent::IsComponentGrabbed(UObject* checkObject)
{
	for (int i = 0; i < handsArr.Num(); ++i)
	{
		if (handsArr[i].grabbedObject == checkObject)
		{
			return true;
		}
	}
	return false;
}

void UOctopusBackpackComponent::Drop(int handIndex)
{
	if (handsArr.IsValidIndex(handIndex) && IsValid(backPackOcto) && bBattleMode && bRegistred)
	{
		if (handsArr[handIndex].action == EOctoHandAction::GRABBED_RETURN || handsArr[handIndex].action == EOctoHandAction::GRABBED)
		{
			UPrimitiveComponent* primitiveComponent_ = Cast<UPrimitiveComponent>(handsArr[handIndex].grabbedObject);
			if (primitiveComponent_)
			{
				FDetachmentTransformRules detachRule_(FDetachmentTransformRules::KeepWorldTransform);
				primitiveComponent_->DetachFromComponent(detachRule_);
				primitiveComponent_->SetSimulatePhysics(true);
			}
			handsArr[handIndex].grabbedObject = nullptr;
			handsArr[handIndex].action = EOctoHandAction::IDLE;
		}
	}
}

void UOctopusBackpackComponent::Throw(int handIndex, FHitResult targetHitResult, float addThrowImpulse)
{
	if (IsValid(backPackOcto) && bBattleMode && bRegistred)
	{
		if (handsArr.IsValidIndex(handIndex))
		{
			if (handsArr[handIndex].action == EOctoHandAction::GRABBED_RETURN || handsArr[handIndex].action == EOctoHandAction::GRABBED)
			{
				handsArr[handIndex].action = EOctoHandAction::THROW;
				handsArr[handIndex].handBeforeMove = handsArr[handIndex].handTransform;
				handsArr[handIndex].throwImpulse = addThrowImpulse;
				backPackOcto->SetStretchLinearLimitsForTentacle(handIndex, true, EOctoHandAction::ATTACKS);

				if (targetHitResult.bBlockingHit)
				{
					FRotator targetRot_ = UKismetMathLibrary::FindLookAtRotation(handsArr[handIndex].handTransform.GetLocation(), targetHitResult.ImpactPoint);
					handsArr[handIndex].targetPoint.SetLocation(targetRot_.Vector() * throwDistanceHandMove + handsArr[handIndex].handTransform.GetLocation());
					handsArr[handIndex].targetPoint.SetRotation(targetRot_.Quaternion());
				}
				else
				{
					FRotator targetRot_ = UKismetMathLibrary::FindLookAtRotation(handsArr[handIndex].handTransform.GetLocation(), targetHitResult.TraceEnd);
					handsArr[handIndex].targetPoint.SetLocation(targetRot_.Vector() * throwDistanceHandMove + handsArr[handIndex].handTransform.GetLocation());
					handsArr[handIndex].targetPoint.SetRotation(targetRot_.Quaternion());
				}
			}
		}
	}
}

void UOctopusBackpackComponent::DropAll()
{
	if (IsValid(backPackOcto) && bBattleMode && bRegistred)
	{
		for (int i = 0; i < handsArr.Num(); ++i)
		{
			if (handsArr[i].action == EOctoHandAction::GRABBED_RETURN || handsArr[i].action == EOctoHandAction::GRABBED)
			{
				UPrimitiveComponent* primitiveComponent_ = Cast<UPrimitiveComponent>(handsArr[i].grabbedObject);
				if (primitiveComponent_)
				{
					FDetachmentTransformRules detachRule_(FDetachmentTransformRules::KeepWorldTransform);
					primitiveComponent_->DetachFromComponent(detachRule_);
					primitiveComponent_->SetSimulatePhysics(true);
				}

				handsArr[i].grabbedObject = nullptr;
				handsArr[i].action = EOctoHandAction::IDLE;
			}
		}
	}
}

void UOctopusBackpackComponent::RegisterBackpack(AOctopusBackpackActor* backpack)
{
	if (IsValid(backpack))
	{
		myCharacter = Cast<ACharacter>(GetOwner());
		backPackOcto = backpack;

		if (backPackOcto)
		{
			InitializeOcto();
			bRegistred = true;
		}
	}
}


void UOctopusBackpackComponent::SetMovementDirection(FVector setInputScale)
{
	inputScale = setInputScale;

	inputScale.X = inputScale.X * 1.f;
	inputScale.Y = inputScale.Y * 1.f;

	FRotator rightRot_ = myCharacter->GetControlRotation();
	rightRot_.Pitch = 0.f;
	FVector right_ = UKismetMathLibrary::GetRightVector(rightRot_);

	right_ = right_ * inputScale.X;

	FRotator forwardRot_ = myCharacter->GetControlRotation();
	forwardRot_.Pitch = 0.f;
	forwardRot_.Roll = 0.f;
	FVector forward_ = UKismetMathLibrary::GetForwardVector(forwardRot_);

	forward_ = forward_ * inputScale.Y;

	movementDirection = forward_ + right_ + FVector(0.f, 0.f, 1.f) * inputScale.Z;
}
