// Copyright 2024 CAS. All Rights Reserved.


#include "OctopusBackpackActor.h"

#include "OctopusBackpackComponent.h"
#include "Components/SphereComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "PhysicsEngine/PhysicsConstraintTemplate.h"

// Sets default values
AOctopusBackpackActor::AOctopusBackpackActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	sceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	sceneComponent->SetMobility(EComponentMobility::Static);
	RootComponent = sceneComponent;

	LeftBottomHandStartMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftBottomHandStartMesh"));
	LeftBottomHandStartMesh->SetupAttachment(RootComponent);
	RightBottomHandStartMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightBottomHandStartMesh"));
	RightBottomHandStartMesh->SetupAttachment(RootComponent);
	LeftTopHandStartMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftTopHandStartMesh"));
	LeftTopHandStartMesh->SetupAttachment(RootComponent);
	RightTopHandStartMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightTopHandStartMesh"));
	RightTopHandStartMesh->SetupAttachment(RootComponent);

	LeftBottomHandStartArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("LeftBottomHandStartArrow"));
	LeftBottomHandStartArrow->SetupAttachment(RootComponent);
	RightBottomHandStartArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("RightBottomHandStartArrow"));
	RightBottomHandStartArrow->SetupAttachment(RootComponent);
	LeftTopHandStartArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("LeftTopHandStartArrow"));
	LeftTopHandStartArrow->SetupAttachment(RootComponent);
	RightTopHandStartArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("RightTopHandStartArrow"));
	RightTopHandStartArrow->SetupAttachment(RootComponent);

	LeftBottomHandGrabArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("LeftBottomHandGrabArrow"));
	LeftBottomHandGrabArrow->SetupAttachment(RootComponent);
	RightBottomHandGrabArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("RightBottomHandGrabArrow"));
	RightBottomHandGrabArrow->SetupAttachment(RootComponent);
	LeftTopHandGrabArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("LeftTopHandGrabArrow"));
	LeftTopHandGrabArrow->SetupAttachment(RootComponent);
	RightTopHandGrabArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("RightTopHandGrabArrow"));
	RightTopHandGrabArrow->SetupAttachment(RootComponent);

	LeftBottomHandWalkArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("LeftBottomHandWalkArrow"));
	LeftBottomHandWalkArrow->SetupAttachment(RootComponent);
	RightBottomHandWalkArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("RightBottomHandWalkArrow"));
	RightBottomHandWalkArrow->SetupAttachment(RootComponent);
	LeftTopHandWalkArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("LeftTopHandWalkArrow"));
	LeftTopHandWalkArrow->SetupAttachment(RootComponent);
	RightTopHandWalkArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("RightTopHandWalkArrow"));
	RightTopHandWalkArrow->SetupAttachment(RootComponent);

	LeftBottomHandFallingArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("LeftBottomHandFallingArrow"));
	LeftBottomHandFallingArrow->SetupAttachment(RootComponent);
	RightBottomHandFallingArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("RightBottomHandFallingArrow"));
	RightBottomHandFallingArrow->SetupAttachment(RootComponent);
	LeftTopHandFallingArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("LeftTopHandFallingArrow"));
	LeftTopHandFallingArrow->SetupAttachment(RootComponent);
	RightTopHandFallingArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("RightTopHandFallingArrow"));
	RightTopHandFallingArrow->SetupAttachment(RootComponent);

	LeftBottomHandWalkGrabArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("LeftBottomHandWalkGrabArrow"));
	LeftBottomHandWalkGrabArrow->SetupAttachment(RootComponent);
	RightBottomHandWalkGrabArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("RightBottomHandWalkGrabArrow"));
	RightBottomHandWalkGrabArrow->SetupAttachment(RootComponent);
	LeftTopHandWalkGrabArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("LeftTopHandWalkGrabArrow"));
	LeftTopHandWalkGrabArrow->SetupAttachment(RootComponent);
	RightTopHandWalkGrabArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("RightTopHandWalkGrabArrow"));
	RightTopHandWalkGrabArrow->SetupAttachment(RootComponent);
}

FTransform AOctopusBackpackActor::GetHandPointTransform(int handID, EOcto_HandPoint handPoint)
{
	FTransform returnTransform_(FTransform::Identity);


	if (tentaclesArr.IsValidIndex(handID))
	{
		switch (handPoint)
		{
		case EOcto_HandPoint::FLY_ORIGIN:
			return tentaclesArr[handID].originArrow->GetComponentTransform();
			break;
		case EOcto_HandPoint::FLY_GRAB_ORIGIN:
			return tentaclesArr[handID].grabArrow->GetComponentTransform();
			break;
		case EOcto_HandPoint::WALK_ORIGIN:
			return tentaclesArr[handID].walkArrow->GetComponentTransform();
			break;
		case EOcto_HandPoint::WALK_GRAB_ORIGIN:
			return tentaclesArr[handID].walkGrabArrow->GetComponentTransform();
			break;
		case EOcto_HandPoint::FALLING_ORIGIN:
			return tentaclesArr[handID].fallingArrow->GetComponentTransform();
			break;
		default:
			break;
		}
	}
	return returnTransform_;
}

void AOctopusBackpackActor::EnableIdleEffect(int handIndex, bool bEnable)
{
	if (effectActorsArr.Num() > 0)
	{
		int randID_ = FMath::RandRange(0, effectActorsArr.Num() - 1);

		if (IsValid(tentaclesArr[handIndex].effectActor))
		{
			tentaclesArr[handIndex].effectActor->Destroy();
		}

		if (bEnable)
		{
			FActorSpawnParameters spawnInfo_;
			spawnInfo_.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			AActor* effectActor_ = GetWorld()->SpawnActor<AActor>(effectActorsArr[randID_], tentaclesArr[handIndex].handSkeletalComponent->GetComponentTransform(), spawnInfo_);
			if (effectActor_)
			{
				tentaclesArr[handIndex].effectActor = effectActor_;
				effectActor_->AttachToComponent(tentaclesArr[handIndex].handSkeletalComponent, FAttachmentTransformRules::KeepWorldTransform, "pelvis");
			}
		}
	}
}

void AOctopusBackpackActor::SetSphereParameters(USphereComponent* sphereComp)
{
	sphereComp->SetVisibility(true);
	sphereComp->SetHiddenInGame(true);
	sphereComp->SetSphereRadius(sphereRadius);

	sphereComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	sphereComp->SetCollisionObjectType(ECC_Destructible);
	sphereComp->SetCollisionResponseToChannels(ECollisionResponse::ECR_Block);
	sphereComp->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	sphereComp->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	
	sphereComp->CanCharacterStepUpOn = ECB_No;
	
	sphereComp->SetEnableGravity(false);
	sphereComp->SetAllMassScale(100000.f);
	
	sphereComp->SetLinearDamping(physDamping_);
	sphereComp->SetAngularDamping(physDamping_);
	sphereComp->BodyInstance.bUseCCD = false;

	sphereComp->SetComponentTickEnabledAsync(true);
	//	sphereComp->SetTickGroup(ETickingGroup::TG_PrePhysics);
}

void AOctopusBackpackActor::SetConstraintParameters(UPhysicsConstraintComponent* constraintComp)
{
	constraintComp->SetDisableCollision(true);

	constraintComp->SetComponentTickEnabledAsync(true);
	//	constraintComp->SetTickGroup(ETickingGroup::TG_PrePhysics);
}

void AOctopusBackpackActor::GenerateHandByLength(float length, int handID)
{
	int lastSphereIndex_ = tentaclesArr[handID].sphereCompsArr.Num() - 1;
	tentaclesArr[handID].sphereCompsArr[lastSphereIndex_]->SetWorldLocation(tentaclesArr[handID].startHandMesh->GetComponentLocation() + tentaclesArr[handID].startHandMesh->GetForwardVector() * (sphereDistanceLoc * lastSphereIndex_));
	tentaclesArr[handID].sphereCompsArr[lastSphereIndex_]->SetWorldRotation(tentaclesArr[handID].startHandMesh->GetComponentRotation());

	USphereComponent* lastSphereComponent_ = tentaclesArr[handID].sphereCompsArr[lastSphereIndex_];
	USphereComponent* firstSphereComponent_ = tentaclesArr[handID].sphereCompsArr[0];

	for (int i = 0; i < tentaclesArr[handID].sphereCompsArr.Num(); ++i)
	{
		if (i != lastSphereIndex_ && i != 0)
		{
			tentaclesArr[handID].sphereCompsArr[i]->DestroyComponent();
		}
	}
	tentaclesArr[handID].sphereCompsArr.Empty();
	tentaclesArr[handID].sphereCompsArr.Add(firstSphereComponent_);

	int section_ = FMath::RoundToInt(length / sphereDistanceLoc);

	for (int i = 0; i < section_; ++i)
	{
		USphereComponent* sphereComp_ = NewObject<USphereComponent>(this, USphereComponent::StaticClass());
		if (sphereComp_)
		{
			sphereComp_->SetupAttachment(tentaclesArr[handID].startHandMesh);

			sphereComp_->SetSimulatePhysics(true);
			sphereComp_->SetMassOverrideInKg("", sphereMass);

			SetSphereParameters(sphereComp_);

			sphereComp_->RegisterComponent();

			FVector sphereLoc_(tentaclesArr[handID].startHandMesh->GetComponentLocation() + tentaclesArr[handID].startHandMesh->GetForwardVector() * sphereDistanceLoc * i);

			sphereComp_->SetWorldLocation(sphereLoc_);

			tentaclesArr[handID].sphereCompsArr.Add(sphereComp_);
		}
	}
	lastSphereComponent_->SetWorldLocation(tentaclesArr[handID].startHandMesh->GetComponentLocation() + tentaclesArr[handID].startHandMesh->GetForwardVector() * sphereDistanceLoc * section_);
	tentaclesArr[handID].sphereCompsArr.Add(lastSphereComponent_);

	for (int i = 0; i < tentaclesArr[handID].constraintCompsArr.Num(); ++i)
	{
		if (IsValid(tentaclesArr[handID].constraintCompsArr[i]))
		{
			tentaclesArr[handID].constraintCompsArr[i]->DestroyComponent();
		}
	}

	tentaclesArr[handID].constraintCompsArr.Empty();

	for (int i = 0; i < tentaclesArr[handID].sphereCompsArr.Num(); ++i)
	{
		if (i < tentaclesArr[handID].sphereCompsArr.Num() - 1)
		{
			UPhysicsConstraintComponent* constraintComp_ = NewObject<UPhysicsConstraintComponent>(this, UPhysicsConstraintComponent::StaticClass());
			if (constraintComp_)
			{
				constraintComp_->RegisterComponent();

				FVector constraintLocation_ = (tentaclesArr[handID].sphereCompsArr[i]->GetComponentLocation() + tentaclesArr[handID].sphereCompsArr[i + 1]->GetComponentLocation()) / 2.f;
				constraintComp_->SetWorldLocation(constraintLocation_);
				constraintComp_->SetConstrainedComponents(tentaclesArr[handID].sphereCompsArr[i + 1], FName(""), tentaclesArr[handID].sphereCompsArr[i], FName(""));

				SetConstraintParameters(constraintComp_);

				if (handAngularLimitCurve)
				{
					float angularLimits_(handAngularLimitCurve->GetFloatValue(static_cast<float>(i)));

					constraintComp_->SetAngularSwing1Limit(ACM_Limited, angularLimits_);
					constraintComp_->SetAngularSwing2Limit(ACM_Limited, angularLimits_);
					constraintComp_->SetAngularTwistLimit(ACM_Limited, angularLimits_);
				}

				tentaclesArr[handID].constraintCompsArr.Add(constraintComp_);
			}
		}
	}
}

void AOctopusBackpackActor::ActivateBackpack(bool bActivate)
{
	bIsActivateBackpack = bActivate;

	if (bActivate)
	{
		bIsHandsShowed = false;
		GenerateHands();
		SetVisibleHands(false);

		for (int i = 0; i < tentaclesArr.Num(); ++i)
		{
			tentaclesArr[i].splineStartLength = tentaclesArr[i].splineComponent->GetSplineLength();
			tentaclesArr[i].splineHideTime = 0.f;
			tentaclesArr[i].junctionInstancedComp->ClearInstances();
		}

		ActivateBackpackEvent(true);
	}
	else
	{
		for (int iTcle_ = 0; iTcle_ < tentaclesArr.Num(); ++iTcle_)
		{
			FOctoTentaclesStruct& tentacle_ = tentaclesArr[iTcle_];			
			for (int i = 0; i < tentacle_.sphereCompsArr.Num(); ++i)
			{
				if (IsValid(tentacle_.sphereCompsArr[i]))
				{
					tentacle_.sphereCompsArr[i]->DestroyComponent();
				}
			}

			for (int i = 0; i < tentacle_.splineMeshCompsArr.Num(); ++i)
			{
				tentacle_.splineMeshCompsArr[i]->DestroyComponent();
			}

			tentacle_.splineMeshCompsArr.Empty();

			for (int i = 0; i < tentacle_.splineMeshBigCompsArr.Num(); ++i)
			{
				tentacle_.splineMeshBigCompsArr[i]->DestroyComponent();
			}

			tentacle_.splineMeshBigCompsArr.Empty();

			USplineComponent* splineComp_ = NewObject<USplineComponent>(this, USplineComponent::StaticClass());
			if (splineComp_)
			{
				splineComp_->SetupAttachment(RootComponent);
				splineComp_->RegisterComponent();
				splineComp_->ClearSplinePoints();

				for (int i = 0; i < tentacle_.splineComponent->GetNumberOfSplinePoints(); ++i)
				{
					splineComp_->AddSplinePoint(tentacle_.splineComponent->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::World), ESplineCoordinateSpace::World);
				}

				tentacle_.splineComponent->DestroyComponent();

				tentacle_.splineComponent = splineComp_;

				tentacle_.splineStartLength = splineComp_->GetSplineLength();
				tentacle_.splineCurrentLength = tentacle_.splineStartLength;
				tentacle_.splineStartPoint = splineComp_->GetNumberOfSplinePoints() - 1;
				tentacle_.splineHideTime = 0.f;

				for (int i = 0; i < tentacle_.splineComponent->GetNumberOfSplinePoints(); ++i)
				{
					USplineMeshComponent* splineMeshComp_ = NewObject<USplineMeshComponent>(this, USplineMeshComponent::StaticClass());
					if (splineMeshComp_)
					{
						splineMeshComp_->SetupAttachment(RootComponent);
						splineMeshComp_->SetStartPosition(tentacle_.splineComponent->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local));
						splineMeshComp_->SetStartTangent(tentacle_.splineComponent->GetDirectionAtSplinePoint(i, ESplineCoordinateSpace::Local));
						splineMeshComp_->SetEndPosition(tentacle_.splineComponent->GetLocationAtSplinePoint(i + 1, ESplineCoordinateSpace::Local));
						splineMeshComp_->SetEndTangent(tentacle_.splineComponent->GetDirectionAtSplinePoint(i, ESplineCoordinateSpace::Local));
						splineMeshComp_->SetStaticMesh(tentaclesStaticMesh);
						splineMeshComp_->SetForwardAxis(ESplineMeshAxis::X);
						splineMeshComp_->RegisterComponent();

						tentacle_.splineMeshCompsArr.Add(splineMeshComp_);
					}

					USplineMeshComponent* splineMeshBigComp_ = NewObject<USplineMeshComponent>(this, USplineMeshComponent::StaticClass());
					if (splineMeshBigComp_)
					{
						splineMeshBigComp_->SetupAttachment(RootComponent);
						splineMeshBigComp_->SetStartPosition(tentacle_.splineComponent->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local));
						splineMeshBigComp_->SetStartTangent(tentacle_.splineComponent->GetDirectionAtSplinePoint(i, ESplineCoordinateSpace::Local));
						splineMeshBigComp_->SetEndPosition(tentacle_.splineComponent->GetLocationAtSplinePoint(i + 1, ESplineCoordinateSpace::Local));
						splineMeshBigComp_->SetEndTangent(tentacle_.splineComponent->GetDirectionAtSplinePoint(i, ESplineCoordinateSpace::Local));
						splineMeshBigComp_->SetStaticMesh(tentaclesBigStaticMesh);
						splineMeshBigComp_->SetForwardAxis(ESplineMeshAxis::X);
						splineMeshBigComp_->RegisterComponent();

						tentacle_.splineMeshBigCompsArr.Add(splineMeshBigComp_);
					}
				}
			}
		}

		ActivateBackpackEvent(false);
	}
}

// Called when the game starts or when spawned
void AOctopusBackpackActor::BeginPlay()
{
	Super::BeginPlay();

	maxBrokenHandDistanceSquared = FMath::Square(maxBrokenHandDistance);

	if (IsValid(GetParentActor()))
	{
		TInlineComponentArray<UOctopusBackpackComponent*> sceneComponentsArr_;
		GetParentActor()->GetComponents(UOctopusBackpackComponent::StaticClass(), sceneComponentsArr_);
		if (sceneComponentsArr_.Num() > 0)
		{
			if (Cast<UOctopusBackpackComponent>(sceneComponentsArr_[0]))
			{
				backpackComponent = sceneComponentsArr_[0];

				//	GenerateHands();

				backpackComponent->RegisterBackpack(this);
			}
		}
	}
}

// Called every frame
void AOctopusBackpackActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!backpackComponent)
	{
		return;
	}

	if (!bIsHandsShowed)
	{
		ShowHandsTick(DeltaTime);
		return;
	}

	if (!bIsActivateBackpack)
	{
		HideHandsTick(DeltaTime);
		return;
	}

	for (int iHand_ = 0; iHand_ < tentaclesArr.Num(); ++iHand_)
	{
		tentaclesArr[iHand_].splineComponent->ClearSplinePoints();
		bool bBrokenHand_(false);
		for (int i = 0; i < tentaclesArr[iHand_].sphereCompsArr.Num(); ++i)
		{
			if (i == tentaclesArr[iHand_].sphereCompsArr.Num() - 1)
			{
				if (backpackComponent->handsArr.IsValidIndex(iHand_))
				{
					tentaclesArr[iHand_].sphereCompsArr[i]->SetWorldLocation(backpackComponent->handsArr[iHand_].handTransform.GetLocation());
					tentaclesArr[iHand_].sphereCompsArr[i]->SetWorldRotation(backpackComponent->handsArr[iHand_].handTransform.GetRotation());
					tentaclesArr[iHand_].handSkeletalComponent->SetWorldLocation(tentaclesArr[iHand_].sphereCompsArr[i]->GetComponentLocation() + tentaclesArr[iHand_].sphereCompsArr[i]->GetForwardVector() * -5.f);
					//(backpackComponent->handsArr[iHand_].handTransform.GetLocation());

					//tentaclesArr[iHand_].handSkeletalComponent->SetWorldRotation(backpackComponent->handsArr[iHand_].handTransform.GetRotation());

					FRotator handRot_ = backpackComponent->handsArr[iHand_].handTransform.GetRotation().Rotator();
					if (backpackComponent->handsArr[iHand_].action == EOctoHandAction::MOVE_TO_ATTACH_POINT)
					{
						if (tentaclesArr[iHand_].newYawRotation == 0.f)
						{
							tentaclesArr[iHand_].newYawRotation = FMath::RandRange(0.f, 360.f);
						}
						tentaclesArr[iHand_].handSkeletalComponent->SetRelativeRotation(FRotator(handRot_.Pitch, handRot_.Yaw, tentaclesArr[iHand_].newYawRotation));
					}
					else if (backpackComponent->handsArr[iHand_].action == EOctoHandAction::ATTACHED)
					{
						tentaclesArr[iHand_].handSkeletalComponent->SetRelativeRotation(FRotator(handRot_.Pitch, handRot_.Yaw, tentaclesArr[iHand_].newYawRotation));
					}
					else
					{
						tentaclesArr[iHand_].handSkeletalComponent->SetWorldRotation(backpackComponent->handsArr[iHand_].handTransform.GetRotation());
					}
				}
			}

			if ((tentaclesArr[iHand_].sphereCompsArr[i]->GetComponentLocation() - GetActorLocation()).SizeSquared() > maxBrokenHandDistanceSquared)
			{
				bBrokenHand_ = true;
			}

			tentaclesArr[iHand_].splineComponent->AddSplinePoint(tentaclesArr[iHand_].sphereCompsArr[i]->GetComponentLocation(), ESplineCoordinateSpace::World, (i == tentaclesArr[iHand_].sphereCompsArr.Num() - 1) ? true : false);
			tentaclesArr[iHand_].splineComponent->SetRotationAtSplinePoint(tentaclesArr[iHand_].splineComponent->GetNumberOfSplinePoints() - 1,
			                                                               tentaclesArr[iHand_].sphereCompsArr[i]->GetComponentRotation(), ESplineCoordinateSpace::World, false);
		}

		if (bBrokenHand_)
		{
			int lastSphereIndex_ = tentaclesArr[iHand_].sphereCompsArr.Num() - 1;
			tentaclesArr[iHand_].sphereCompsArr[lastSphereIndex_]->SetWorldLocation(tentaclesArr[iHand_].startHandMesh->GetComponentLocation() +
				tentaclesArr[iHand_].startHandMesh->GetForwardVector() * (sphereDistanceLoc * lastSphereIndex_));
			tentaclesArr[iHand_].sphereCompsArr[lastSphereIndex_]->SetWorldRotation(tentaclesArr[iHand_].startHandMesh->GetComponentRotation());

			for (int i = 0; i < tentaclesArr[iHand_].sphereCompsArr.Num(); ++i)
			{
				if (i != 0 && i != lastSphereIndex_)
				{
					if (IsValid(tentaclesArr[iHand_].sphereCompsArr[i]))
					{
						tentaclesArr[iHand_].sphereCompsArr[i]->DestroyComponent();
						tentaclesArr[iHand_].sphereCompsArr[i] = nullptr;
					}

					USphereComponent* sphereComp_ = NewObject<USphereComponent>(this, USphereComponent::StaticClass());
					if (sphereComp_)
					{
						sphereComp_->SetupAttachment(tentaclesArr[iHand_].startHandMesh);

						sphereComp_->SetSimulatePhysics(true);
						sphereComp_->SetMassOverrideInKg("", sphereMass);

						SetSphereParameters(sphereComp_);

						sphereComp_->RegisterComponent();

						FVector sphereLoc_(tentaclesArr[iHand_].startHandMesh->GetComponentLocation() + tentaclesArr[iHand_].startHandMesh->GetForwardVector() * sphereDistanceLoc * i);

						sphereComp_->SetWorldLocation(sphereLoc_);

						tentaclesArr[iHand_].sphereCompsArr[i] = sphereComp_;
					}
				}
			}

			for (int i = 0; i < tentaclesArr[iHand_].constraintCompsArr.Num(); ++i)
			{
				if (IsValid(tentaclesArr[iHand_].constraintCompsArr[i]))
				{
					tentaclesArr[iHand_].constraintCompsArr[i]->DestroyComponent();
				}
			}

			tentaclesArr[iHand_].constraintCompsArr.Empty();

			for (int i = 0; i < tentaclesArr[iHand_].sphereCompsArr.Num(); ++i)
			{
				if (i < tentaclesArr[iHand_].sphereCompsArr.Num() - 1)
				{
					UPhysicsConstraintComponent* constraintComp_ = NewObject<UPhysicsConstraintComponent>(this, UPhysicsConstraintComponent::StaticClass());
					if (constraintComp_)
					{
						constraintComp_->RegisterComponent();

						FVector constraintLocation_ = (tentaclesArr[iHand_].sphereCompsArr[i]->GetComponentLocation() + tentaclesArr[iHand_].sphereCompsArr[i + 1]->GetComponentLocation()) / 2.f;
						constraintComp_->SetWorldLocation(constraintLocation_);
						constraintComp_->SetConstrainedComponents(tentaclesArr[iHand_].sphereCompsArr[i + 1], FName(""), tentaclesArr[iHand_].sphereCompsArr[i], FName(""));

						SetConstraintParameters(constraintComp_);

						if (handAngularLimitCurve)
						{
							float angularLimits_(handAngularLimitCurve->GetFloatValue(static_cast<float>(i)));

							constraintComp_->SetAngularSwing1Limit(ACM_Limited, angularLimits_);
							constraintComp_->SetAngularSwing2Limit(ACM_Limited, angularLimits_);
							constraintComp_->SetAngularTwistLimit(ACM_Limited, angularLimits_);
						}

						tentaclesArr[iHand_].constraintCompsArr.Add(constraintComp_);
					}
				}
			}
		}

		USplineComponent* splineComp_ = tentaclesArr[iHand_].splineComponent;
		for (int iTcs_ = 0; iTcs_ < tentaclesArr[iHand_].splineMeshCompsArr.Num(); ++iTcs_)
		{
			USplineMeshComponent* splineMeshComp_ = tentaclesArr[iHand_].splineMeshCompsArr[iTcs_];
			USplineMeshComponent* splineMeshBigComp_ = tentaclesArr[iHand_].splineMeshBigCompsArr[iTcs_];
			if (splineMeshComp_ && splineComp_ && splineMeshBigComp_)
			{
				int nextPoint_ = iTcs_ + 1;
				FVector startPointLoc_;
				FVector startPointTangent_;
				FVector endPointLoc_;
				FVector endPointTangent_;
				splineComp_->GetLocationAndTangentAtSplinePoint(iTcs_, startPointLoc_, startPointTangent_, ESplineCoordinateSpace::Local);
				splineComp_->GetLocationAndTangentAtSplinePoint(nextPoint_, endPointLoc_, endPointTangent_, ESplineCoordinateSpace::Local);

				splineMeshComp_->SetStartTangent(startPointTangent_, false);
				splineMeshComp_->SetEndTangent(endPointTangent_, false);

				splineMeshComp_->SetStartPosition(startPointLoc_, false);
				splineMeshComp_->SetEndPosition(endPointLoc_, false);

				splineMeshComp_->SetStartRoll(splineComp_->GetRotationAtSplinePoint(iTcs_, ESplineCoordinateSpace::Local).Roll / 57.f);
				splineMeshComp_->SetEndRoll(splineComp_->GetRotationAtSplinePoint(nextPoint_, ESplineCoordinateSpace::Local).Roll / 57.f);

				//	splineMeshComponent_->SetSplineUpDir(splineComponent_->GetUpVectorAtSplinePoint(iTcs_, ESplineCoordinateSpace::World), false);
				splineMeshComp_->UpdateMesh();

				splineMeshBigComp_->SetStartTangent(startPointTangent_, false);
				splineMeshBigComp_->SetEndTangent(endPointTangent_, false);

				splineMeshBigComp_->SetStartPosition(startPointLoc_, false);
				splineMeshBigComp_->SetEndPosition(endPointLoc_, false);

				// splineMeshBigComponent_->SetStartRoll(splineComponent_->GetRotationAtSplinePoint(iTcs_, ESplineCoordinateSpace::Local).Roll / 57.f);
				// splineMeshBigComponent_->SetEndRoll(splineComponent_->GetRotationAtSplinePoint(nextPoint_, ESplineCoordinateSpace::Local).Roll / 57.f);

				splineMeshBigComp_->SetSplineUpDir(splineComp_->GetUpVectorAtSplinePoint(iTcs_, ESplineCoordinateSpace::World), false);

				splineMeshBigComp_->UpdateMesh();
			}
		}

		float newSpacer_ = (tentaclesArr[iHand_].splineComponent->GetSplineLength() - junctionEndSpace) / junctionCount;
		for (int i = 0; i < tentaclesArr[iHand_].junctionInstancedComp->GetInstanceCount(); ++i)
		{
			FTransform junctionTransform_(FTransform::Identity);
			if (i == 0)
			{
				junctionTransform_.SetIdentityZeroScale();
			}
			else
			{
				junctionTransform_ = tentaclesArr[iHand_].splineComponent->GetTransformAtDistanceAlongSpline(newSpacer_ * i, ESplineCoordinateSpace::Local);
			}
			tentaclesArr[iHand_].junctionInstancedComp->UpdateInstanceTransform(i, junctionTransform_);
		}
		tentaclesArr[iHand_].junctionInstancedComp->MarkRenderStateDirty();
	}
}

void AOctopusBackpackActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	//	GenerateHands();
}

void AOctopusBackpackActor::GenerateHands()
{
	ClearHands();

	sphereDistanceLoc = sphereRadius * 2.f + sphereSpacer;


	for (int i = 0; i < 4; ++i)
	{
		FOctoTentaclesStruct tentacles_;
		tentaclesArr.Add(tentacles_);
	}

	GenerateOneHand(tentaclesArr[0], LeftBottomHandStartMesh, EOcto_Hand::LEFT_BOTTOM, 0);
	GenerateOneHand(tentaclesArr[1], RightBottomHandStartMesh, EOcto_Hand::RIGHT_BOTTOM, 1);
	GenerateOneHand(tentaclesArr[2], LeftTopHandStartMesh, EOcto_Hand::LEFT_TOP, 2);
	GenerateOneHand(tentaclesArr[3], RightTopHandStartMesh, EOcto_Hand::RIGHT_TOP, 3);

	bIsActivateBackpack = true;
}

void AOctopusBackpackActor::GenerateOneHand(FOctoTentaclesStruct& tentacles, UStaticMeshComponent* handMesh, EOcto_Hand hand, int handIndex)
{
	tentacles.hand = hand;
	tentacles.startHandMesh = handMesh;

	UArrowComponent* arrowComp_ = NewObject<UArrowComponent>(this, UArrowComponent::StaticClass());
	tentacles.targetArrow = arrowComp_;
	tentacles.hand = hand;

	if (IsValid(backpackComponent))
	{
		tentacles.backpackComponent = backpackComponent;
	}
	tentacles.handIndex = handIndex;
	
	switch (hand)
	{
	case EOcto_Hand::LEFT_BOTTOM:
		tentacles.originArrow = LeftBottomHandStartArrow;
		tentacles.grabArrow = LeftBottomHandGrabArrow;
		tentacles.walkArrow = LeftBottomHandWalkArrow;
		tentacles.walkGrabArrow = LeftBottomHandWalkGrabArrow;
		tentacles.fallingArrow = LeftBottomHandFallingArrow;
		arrowComp_->SetupAttachment(tentacles.originArrow);
		arrowComp_->SetWorldLocation(tentacles.originArrow->GetComponentLocation());
		arrowComp_->RegisterComponent();
		break;
	case EOcto_Hand::RIGHT_BOTTOM:
		tentacles.originArrow = RightBottomHandStartArrow;
		tentacles.grabArrow = RightBottomHandGrabArrow;
		tentacles.walkArrow = RightBottomHandWalkArrow;
		tentacles.walkGrabArrow = RightBottomHandWalkGrabArrow;
		tentacles.fallingArrow = RightBottomHandFallingArrow;
		arrowComp_->SetupAttachment(tentacles.originArrow);
		arrowComp_->SetWorldLocation(tentacles.originArrow->GetComponentLocation());
		arrowComp_->RegisterComponent();
		break;
	case EOcto_Hand::LEFT_TOP:
		tentacles.originArrow = LeftTopHandStartArrow;
		tentacles.grabArrow = LeftTopHandGrabArrow;
		tentacles.walkArrow = LeftTopHandWalkArrow;
		tentacles.walkGrabArrow = LeftTopHandWalkGrabArrow;
		tentacles.fallingArrow = LeftTopHandFallingArrow;
		arrowComp_->SetupAttachment(tentacles.originArrow);
		arrowComp_->SetWorldLocation(tentacles.originArrow->GetComponentLocation());
		arrowComp_->RegisterComponent();
		break;
	case EOcto_Hand::RIGHT_TOP:
		tentacles.originArrow = RightTopHandStartArrow;
		tentacles.grabArrow = RightTopHandGrabArrow;
		tentacles.walkArrow = RightTopHandWalkArrow;
		tentacles.walkGrabArrow = RightTopHandWalkGrabArrow;
		tentacles.fallingArrow = RightTopHandFallingArrow;
		arrowComp_->SetupAttachment(tentacles.originArrow);
		arrowComp_->SetWorldLocation(tentacles.originArrow->GetComponentLocation());
		arrowComp_->RegisterComponent();
		break;
	default:
		break;
	}

	for (int i = 0; i < sectionCount; ++i)
	{
		USphereComponent* sphereComp_ = NewObject<USphereComponent>(this, USphereComponent::StaticClass());
		if (sphereComp_)
		{
			sphereComp_->SetupAttachment(handMesh);

			SetSphereParameters(sphereComp_);

			if (i != 0 && i != sectionCount - 1)
			{
				//	sphereComp_->SetSimulatePhysics(true);
				sphereComp_->SetMassOverrideInKg("", sphereMass);
			}
			sphereComp_->RegisterComponent();

			FVector sphereLoc_ = handMesh->GetComponentLocation() + handMesh->GetForwardVector() * sphereDistanceLoc * i;

			sphereComp_->SetWorldLocation(sphereLoc_);

			tentacles.sphereCompsArr.Add(sphereComp_);

			if (i == sectionCount - 1)
			{
				if (handSkeletalMesh)
				{
					USkeletalMeshComponent* handSkeletalComp_ = NewObject<USkeletalMeshComponent>(this, USkeletalMeshComponent::StaticClass());
					if (handSkeletalComp_)
					{
						FVector handLoc_(handMesh->GetComponentLocation() + handMesh->GetForwardVector() * (sphereDistanceLoc * (i + 1) - sphereRadius));
						handSkeletalComp_->SetWorldLocation(handLoc_);
						handSkeletalComp_->SetSkeletalMesh(handSkeletalMesh);
						handSkeletalComp_->SetCollisionEnabled(ECollisionEnabled::NoCollision);

						if (leftBottomHandAnimationClass && rightBottomHandAnimationClass && leftTopHandAnimationClass && rightTopHandAnimationClass)
						{
							UAnimInstance* animInst_ = nullptr;

							switch (hand)
							{
							case EOcto_Hand::LEFT_BOTTOM:
								animInst_ = NewObject<UAnimInstance>(handSkeletalComp_, leftBottomHandAnimationClass->GeneratedClass);
								break;
							case EOcto_Hand::RIGHT_BOTTOM:
								animInst_ = NewObject<UAnimInstance>(handSkeletalComp_, rightBottomHandAnimationClass->GeneratedClass);
								break;
							case EOcto_Hand::LEFT_TOP:
								animInst_ = NewObject<UAnimInstance>(handSkeletalComp_, leftTopHandAnimationClass->GeneratedClass);
								break;
							case EOcto_Hand::RIGHT_TOP:
								animInst_ = NewObject<UAnimInstance>(handSkeletalComp_, rightTopHandAnimationClass->GeneratedClass);
								break;
							default:
								break;
							}

							if (animInst_)
							{
								handSkeletalComp_->SetAnimationMode(EAnimationMode::AnimationBlueprint);
								handSkeletalComp_->SetAnimInstanceClass(animInst_->GetClass());
							}
						}
						handSkeletalComp_->RegisterComponent();
						tentacles.handSkeletalComponent = handSkeletalComp_;
					}
				}
			}
		}
	}

	for (int i = 0; i < tentacles.sphereCompsArr.Num(); ++i)
	{
		if (i < tentacles.sphereCompsArr.Num() - 1)
		{
			UPhysicsConstraintComponent* constraintComp_ = NewObject<UPhysicsConstraintComponent>(this, UPhysicsConstraintComponent::StaticClass());
			if (constraintComp_)
			{
				constraintComp_->RegisterComponent();

				FVector constraintLocation_ = (tentacles.sphereCompsArr[i]->GetComponentLocation() + tentacles.sphereCompsArr[i + 1]->GetComponentLocation()) / 2.f;
				constraintComp_->SetWorldLocation(constraintLocation_);
				constraintComp_->SetConstrainedComponents(tentacles.sphereCompsArr[i + 1], FName(""), tentacles.sphereCompsArr[i], FName(""));

				SetConstraintParameters(constraintComp_);

				if (handAngularLimitCurve)
				{
					float angularLimits_(handAngularLimitCurve->GetFloatValue(static_cast<float>(i)));

					constraintComp_->SetAngularSwing1Limit(ACM_Limited, angularLimits_);
					constraintComp_->SetAngularSwing2Limit(ACM_Limited, angularLimits_);
					constraintComp_->SetAngularTwistLimit(ACM_Limited, angularLimits_);
				}

				tentacles.constraintCompsArr.Add(constraintComp_);
			}
		}
	}

	for (int i = 0; i < tentacles.sphereCompsArr.Num(); ++i)
	{
		if (i < tentacles.sphereCompsArr.Num() - 1)
		{
			if (tentaclesStaticMesh)
			{
				USplineMeshComponent* splineMeshComp_ = NewObject<USplineMeshComponent>(this, USplineMeshComponent::StaticClass());
				if (splineMeshComp_)
				{
					splineMeshComp_->SetStartPosition(tentacles.sphereCompsArr[i]->GetComponentLocation());
					splineMeshComp_->SetStartTangent(tentacles.sphereCompsArr[i]->GetForwardVector());
					splineMeshComp_->SetEndPosition(tentacles.sphereCompsArr[i + 1]->GetComponentLocation());
					splineMeshComp_->SetEndTangent(tentacles.sphereCompsArr[i]->GetForwardVector());
					splineMeshComp_->SetStaticMesh(tentaclesStaticMesh);
					splineMeshComp_->SetForwardAxis(ESplineMeshAxis::X);

					splineMeshComp_->RegisterComponent();

					tentacles.splineMeshCompsArr.Add(splineMeshComp_);
				}
			}

			if (tentaclesBigStaticMesh)
			{
				USplineMeshComponent* splineMeshBigComp_ = NewObject<USplineMeshComponent>(this, USplineMeshComponent::StaticClass());
				if (splineMeshBigComp_)
				{
					splineMeshBigComp_->SetStartPosition(tentacles.sphereCompsArr[i]->GetComponentLocation());
					splineMeshBigComp_->SetStartTangent(tentacles.sphereCompsArr[i]->GetForwardVector());
					splineMeshBigComp_->SetEndPosition(tentacles.sphereCompsArr[i + 1]->GetComponentLocation());
					splineMeshBigComp_->SetEndTangent(tentacles.sphereCompsArr[i]->GetForwardVector());
					splineMeshBigComp_->SetStaticMesh(tentaclesBigStaticMesh);
					splineMeshBigComp_->SetForwardAxis(ESplineMeshAxis::X);

					float scale_ = 1.1f;
					splineMeshBigComp_->SetStartScale(FVector2d(scale_, scale_));
					splineMeshBigComp_->SetEndScale(FVector2d(scale_, scale_));

					splineMeshBigComp_->RegisterComponent();

					tentacles.splineMeshBigCompsArr.Add(splineMeshBigComp_);
				}
			}
		}
	}

	USplineComponent* splineComp_ = NewObject<USplineComponent>(this, USplineComponent::StaticClass());
	if (splineComp_)
	{
		splineComp_->RegisterComponent();
		splineComp_->ClearSplinePoints();

		for (int i = 0; i < tentacles.sphereCompsArr.Num(); ++i)
		{
			splineComp_->AddSplinePoint(tentacles.sphereCompsArr[i]->GetComponentLocation(), ESplineCoordinateSpace::World, (i == tentacles.sphereCompsArr.Num() - 1) ? true : false);
		}

		tentacles.splineComponent = splineComp_;
	}

	if (junctionStaticMesh)
	{
		UInstancedStaticMeshComponent* junctionInstancedComp_ = NewObject<UInstancedStaticMeshComponent>(this, UInstancedStaticMeshComponent::StaticClass());
		if (junctionInstancedComp_)
		{
			junctionInstancedComp_->SetStaticMesh(junctionStaticMesh);
			junctionInstancedComp_->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			junctionInstancedComp_->RegisterComponent();


			// float newSpacer_ = splineComp_->GetSplineLength() / junctionCount;
			// for (int i = 0; i < junctionCount; ++i)
			// {
			// 	float distance_ = newSpacer_ * i;
			// 	junctionInstancedComp_->AddInstance(splineComp_->GetTransformAtDistanceAlongSpline(distance_, ESplineCoordinateSpace::World));
			// }

			tentacles.junctionInstancedComp = junctionInstancedComp_;
		}
	}

	for (int i = 0; i < tentacles.sphereCompsArr.Num(); ++i)
	{
		tentacles.sphereCompsArr[i]->SetWorldLocation(tentacles.sphereCompsArr[0]->GetComponentLocation());
		if (i != 0 && i != tentacles.sphereCompsArr.Num() - 1)
		{
			tentacles.sphereCompsArr[i]->SetSimulatePhysics(true);
		}
	}
}

void AOctopusBackpackActor::ClearHands()
{
	if (!tentaclesArr.IsEmpty())
	{
		for (int iHand_ = 0; iHand_ < tentaclesArr.Num(); ++iHand_)
		{
			for (int i = 0; i < tentaclesArr[iHand_].sphereCompsArr.Num(); ++i)
			{
				if (IsValid(tentaclesArr[iHand_].sphereCompsArr[i]))
				{
					tentaclesArr[iHand_].sphereCompsArr[i]->DestroyComponent();
				}
			}
			for (int i = 0; i < tentaclesArr[iHand_].splineMeshCompsArr.Num(); ++i)
			{
				if (IsValid(tentaclesArr[iHand_].splineMeshCompsArr[i]))
				{
					tentaclesArr[iHand_].splineMeshCompsArr[i]->DestroyComponent();
				}
			}
			for (int i = 0; i < tentaclesArr[iHand_].splineMeshBigCompsArr.Num(); ++i)
			{
				if (IsValid(tentaclesArr[iHand_].splineMeshBigCompsArr[i]))
				{
					tentaclesArr[iHand_].splineMeshBigCompsArr[i]->DestroyComponent();
				}
			}
			for (int i = 0; i < tentaclesArr[iHand_].constraintCompsArr.Num(); ++i)
			{
				if (IsValid(tentaclesArr[iHand_].constraintCompsArr[i]))
				{
					tentaclesArr[iHand_].constraintCompsArr[i]->DestroyComponent();
				}
			}
			if (IsValid(tentaclesArr[iHand_].effectActor))
			{
				tentaclesArr[iHand_].effectActor->Destroy();
			}
			if (IsValid(tentaclesArr[iHand_].splineComponent))
			{
				tentaclesArr[iHand_].splineComponent->DestroyComponent();
			}

			if (IsValid(tentaclesArr[iHand_].handSkeletalComponent))
			{
				tentaclesArr[iHand_].handSkeletalComponent->DestroyComponent();
			}
			if (IsValid(tentaclesArr[iHand_].junctionInstancedComp))
			{
				tentaclesArr[iHand_].junctionInstancedComp->ClearInstances();
				tentaclesArr[iHand_].junctionInstancedComp->DestroyComponent();
			}
		}

		tentaclesArr.Empty();
	}
}

void AOctopusBackpackActor::HideHandsTick(const float& deltaTime)
{
	for (int iHand_ = 0; iHand_ < tentaclesArr.Num(); ++iHand_)
	{
		FOctoTentaclesStruct& tentacle_ = tentaclesArr[iHand_];
		tentacle_.splineHideTime += deltaTime * 1.5f;
		backpackComponent->handsArr[iHand_].action = EOctoHandAction::SHOW_HIDE;

		tentacle_.splineCurrentLength = FMath::Lerp(tentacle_.splineStartLength, 0.f, tentacle_.splineHideTime);

		int lengthJunctionCount_ = FMath::Lerp(junctionCount * 0.7f, 0, tentacle_.splineHideTime);

		if (tentacle_.splineHideTime >= 1.f)
		{
			backpackComponent->handsArr[iHand_].action = EOctoHandAction::IDLE;
			ClearHands();
		}
		else
		{
			int point_ = tentacle_.splineMeshBigCompsArr.Num() - 1;
			tentacle_.splineMeshBigCompsArr[point_]->SetVisibility(false);
			tentacle_.splineMeshCompsArr[point_]->SetVisibility(false);

			USplineComponent* splineComp_ = tentacle_.splineComponent;

			int newJunctionCount_ = FMath::Max(tentacle_.junctionInstancedComp->GetInstanceCount() - lengthJunctionCount_, 0);
			for (int i = 0; i < newJunctionCount_; ++i)
			{
				tentacle_.junctionInstancedComp->RemoveInstance(i);
			}

			float newSpacer_ = (tentacle_.splineCurrentLength - junctionEndSpace) / tentacle_.junctionInstancedComp->GetInstanceCount();
			for (int i = 0; i < tentacle_.junctionInstancedComp->GetInstanceCount(); ++i)
			{
				FTransform junctionTransform_ = splineComp_->GetTransformAtDistanceAlongSpline(newSpacer_ * i, ESplineCoordinateSpace::World);
				tentacle_.junctionInstancedComp->UpdateInstanceTransform(i, junctionTransform_, true);
			}
			tentacle_.junctionInstancedComp->MarkRenderStateDirty();

			for (int iTcs_ = tentacle_.splineMeshCompsArr.Num() - 1; iTcs_ >= 0; --iTcs_)
			{
				USplineMeshComponent* splineMeshComp_ = tentacle_.splineMeshCompsArr[iTcs_];
				USplineMeshComponent* splineMeshBigComp_ = tentacle_.splineMeshBigCompsArr[iTcs_];
				if (splineMeshComp_ && splineComp_ && splineMeshBigComp_)
				{
					tentaclesArr[iHand_].handSkeletalComponent->SetWorldLocation(splineComp_->GetLocationAtDistanceAlongSpline(tentacle_.splineCurrentLength, ESplineCoordinateSpace::World));
					tentaclesArr[iHand_].handSkeletalComponent->SetWorldRotation(splineComp_->GetRotationAtDistanceAlongSpline(tentacle_.splineCurrentLength, ESplineCoordinateSpace::World));

					int nextPoint_ = iTcs_ + 1;
					if (tentacle_.splineCurrentLength <= splineComp_->GetDistanceAlongSplineAtSplinePoint(iTcs_ - 1))
					{
						tentacle_.splineMeshBigCompsArr[iTcs_]->DestroyComponent();
						tentacle_.splineMeshCompsArr[iTcs_]->DestroyComponent();
						tentacle_.splineMeshBigCompsArr.RemoveAt(iTcs_);
						tentacle_.splineMeshCompsArr.RemoveAt(iTcs_);
						splineComp_->RemoveSplinePoint(iTcs_);
					}
					else
					{
						FVector startPointLoc_;
						FVector startPointTangent_;
						FVector endPointTangent_;
						FVector endPointLoc_;
						splineComp_->GetLocationAndTangentAtSplinePoint(iTcs_, startPointLoc_, startPointTangent_, ESplineCoordinateSpace::Local);

						if (nextPoint_ == tentacle_.splineMeshCompsArr.Num() - 1)
						{
							FVector noNeedLoc_;
							splineComp_->GetLocationAndTangentAtSplinePoint(nextPoint_, noNeedLoc_, endPointTangent_, ESplineCoordinateSpace::Local);
							endPointLoc_ = splineComp_->GetLocationAtDistanceAlongSpline(tentacle_.splineCurrentLength, ESplineCoordinateSpace::Local);
						}
						else
						{
							splineComp_->GetLocationAndTangentAtSplinePoint(nextPoint_, endPointLoc_, endPointTangent_, ESplineCoordinateSpace::Local);
						}

						splineMeshComp_->SetStartTangent(startPointTangent_, false);
						splineMeshComp_->SetEndTangent(endPointTangent_, false);

						splineMeshComp_->SetStartPosition(startPointLoc_, false);
						splineMeshComp_->SetEndPosition(endPointLoc_, false);

						splineMeshComp_->SetStartRoll(splineComp_->GetRotationAtSplinePoint(iTcs_, ESplineCoordinateSpace::Local).Roll / 57.f);
						splineMeshComp_->SetEndRoll(splineComp_->GetRotationAtSplinePoint(nextPoint_, ESplineCoordinateSpace::Local).Roll / 57.f);

						splineMeshBigComp_->SetStartTangent(startPointTangent_, false);
						splineMeshBigComp_->SetEndTangent(endPointTangent_, false);

						splineMeshBigComp_->SetStartPosition(startPointLoc_, false);
						splineMeshBigComp_->SetEndPosition(endPointLoc_, false);

						splineMeshComp_->UpdateMesh();
						splineMeshBigComp_->UpdateMesh();
					}
				}
			}
		}
	}
}

void AOctopusBackpackActor::ShowHandsTick(const float& deltaTime)
{
	SetVisibleHands(true);

	for (int iHand_ = 0; iHand_ < tentaclesArr.Num(); ++iHand_)
	{
		FOctoTentaclesStruct& tentacle_ = tentaclesArr[iHand_];
		backpackComponent->handsArr[iHand_].hand = tentaclesArr[iHand_].hand;
		backpackComponent->handsArr[iHand_].action = EOctoHandAction::SHOW_HIDE;

		USplineComponent* splineComp_ = NewObject<USplineComponent>(this, USplineComponent::StaticClass());
		if (splineComp_)
		{
			splineComp_->RegisterComponent();
			splineComp_->ClearSplinePoints();

			tentacle_.splineHideTime += deltaTime * 1.5f;
			tentacle_.splineCurrentLength = FMath::Lerp(0.f, tentacle_.splineStartLength, tentacle_.splineHideTime);

			if (tentacle_.splineHideTime >= 1.f)
			{
				bIsHandsShowed = true;
				backpackComponent->handsArr[iHand_].action = EOctoHandAction::IDLE;
				SetVisibleHands(true);
				tentacle_.junctionInstancedComp->ClearInstances();

				for (int i = 0; i < tentaclesArr[iHand_].sphereCompsArr.Num(); ++i)
				{
					splineComp_->AddSplinePoint(tentaclesArr[iHand_].sphereCompsArr[i]->GetComponentLocation(), ESplineCoordinateSpace::World);
				}
				splineComp_->UpdateSpline();

				GenerateJunction(iHand_, splineComp_);

				splineComp_->DestroyComponent();
			}
			else
			{
				for (int i = 0; i < tentaclesArr[iHand_].sphereCompsArr.Num(); ++i)
				{
					splineComp_->AddSplinePoint(tentaclesArr[iHand_].sphereCompsArr[i]->GetComponentLocation(), ESplineCoordinateSpace::World);
				}
				splineComp_->UpdateSpline();

				int lastSphereID_(tentacle_.sphereCompsArr.Num() - 1);
				tentacle_.sphereCompsArr[lastSphereID_]->SetWorldLocation(backpackComponent->handsArr[iHand_].handTransform.GetLocation());
				tentacle_.sphereCompsArr[lastSphereID_]->SetWorldRotation(backpackComponent->handsArr[iHand_].handTransform.GetRotation());
				tentaclesArr[iHand_].handSkeletalComponent->SetWorldLocation(splineComp_->GetLocationAtDistanceAlongSpline(tentacle_.splineCurrentLength, ESplineCoordinateSpace::World));
				tentaclesArr[iHand_].handSkeletalComponent->SetWorldRotation(splineComp_->GetRotationAtDistanceAlongSpline(tentacle_.splineCurrentLength, ESplineCoordinateSpace::World));

				float newSpacer_(12.f);
				if (tentacle_.splineCurrentLength / newSpacer_ > tentacle_.junctionInstancedComp->GetInstanceCount())
				{
					tentacle_.junctionInstancedComp->AddInstance(splineComp_->GetTransformAtDistanceAlongSpline(tentacle_.splineCurrentLength, ESplineCoordinateSpace::World));
				}

				for (int i = 0; i < tentaclesArr[iHand_].junctionInstancedComp->GetInstanceCount(); ++i)
				{
					FTransform junctionTransform_ = splineComp_->GetTransformAtDistanceAlongSpline(newSpacer_ * i, ESplineCoordinateSpace::World);
					tentaclesArr[iHand_].junctionInstancedComp->UpdateInstanceTransform(i, junctionTransform_);
				}
				tentaclesArr[iHand_].junctionInstancedComp->MarkRenderStateDirty();

				for (int iTcs_ = 0; iTcs_ < tentacle_.splineMeshCompsArr.Num(); ++iTcs_)
				{
					USplineMeshComponent* splineMeshComp_ = tentacle_.splineMeshCompsArr[iTcs_];
					USplineMeshComponent* splineMeshBigComp_ = tentacle_.splineMeshBigCompsArr[iTcs_];
					if (splineMeshComp_ && splineComp_ && splineMeshBigComp_)
					{
						FVector startPointLoc_(tentacle_.startHandMesh->GetComponentLocation());
						FVector endPointLoc_(tentacle_.startHandMesh->GetComponentLocation());
						FVector startPointTangent_(FVector::ZeroVector);
						FVector endPointTangent_(FVector::ZeroVector);

						int nextPoint_ = iTcs_ + 1;
						if (splineComp_->GetDistanceAlongSplineAtSplinePoint(iTcs_) <= tentacle_.splineCurrentLength)
						{
							splineComp_->GetLocationAndTangentAtSplinePoint(iTcs_, startPointLoc_, startPointTangent_, ESplineCoordinateSpace::Local);
							FVector noNeedLoc_;
							splineComp_->GetLocationAndTangentAtSplinePoint(nextPoint_, noNeedLoc_, endPointTangent_, ESplineCoordinateSpace::Local);
							endPointLoc_ = splineComp_->GetLocationAtDistanceAlongSpline(tentacle_.splineCurrentLength, ESplineCoordinateSpace::Local);
						}
						if (splineComp_->GetDistanceAlongSplineAtSplinePoint(nextPoint_) <= tentacle_.splineCurrentLength)
						{
							splineComp_->GetLocationAndTangentAtSplinePoint(nextPoint_, endPointLoc_, endPointTangent_, ESplineCoordinateSpace::Local);
						}

						splineMeshComp_->SetStartTangent(startPointTangent_, false);
						splineMeshComp_->SetEndTangent(endPointTangent_, false);

						splineMeshComp_->SetStartPosition(startPointLoc_, false);
						splineMeshComp_->SetEndPosition(endPointLoc_, false);

						splineMeshComp_->SetStartRoll(splineComp_->GetRotationAtSplinePoint(iTcs_, ESplineCoordinateSpace::Local).Roll / 57.f);
						splineMeshComp_->SetEndRoll(splineComp_->GetRotationAtSplinePoint(nextPoint_, ESplineCoordinateSpace::Local).Roll / 57.f);

						splineMeshBigComp_->SetStartTangent(startPointTangent_, false);
						splineMeshBigComp_->SetEndTangent(endPointTangent_, false);

						splineMeshBigComp_->SetStartPosition(startPointLoc_, false);
						splineMeshBigComp_->SetEndPosition(endPointLoc_, false);

						splineMeshComp_->UpdateMesh();
						splineMeshBigComp_->UpdateMesh();
					}
				}
				splineComp_->DestroyComponent();
			}
		}
	}
}

void AOctopusBackpackActor::SetVisibleHands(bool bVisible)
{
	for (int iHand_ = 0; iHand_ < tentaclesArr.Num(); ++iHand_)
	{
		FOctoTentaclesStruct& tentacle_ = tentaclesArr[iHand_];

		for (int i = 0; i < tentacle_.splineMeshCompsArr.Num(); ++i)
		{
			tentacle_.splineMeshCompsArr[i]->SetVisibility(bVisible);
		}
		for (int i = 0; i < tentacle_.splineMeshBigCompsArr.Num(); ++i)
		{
			tentacle_.splineMeshBigCompsArr[i]->SetVisibility(bVisible);
		}
		tentacle_.handSkeletalComponent->SetVisibility(bVisible);
		tentacle_.junctionInstancedComp->SetVisibility(bVisible);
	}
}

void AOctopusBackpackActor::GenerateJunction(const int& handIndex, const USplineComponent* splineComp)
{
	float newSpacer_ = splineComp->GetSplineLength() / junctionCount;
	for (int i = 0; i < junctionCount; ++i)
	{
		float distance_ = newSpacer_ * i;
		tentaclesArr[handIndex].junctionInstancedComp->AddInstance(splineComp->GetTransformAtDistanceAlongSpline(distance_, ESplineCoordinateSpace::World), true);
	}
}

void AOctopusBackpackActor::SetStretchLinearLimitsForTentacle(int handIndex, bool bStretch, EOctoHandAction handAction)
{
	if (!walkHandStretchCurve || !attackHandStretchCurve)
	{
		return;
	}

	if (tentaclesArr.IsValidIndex(handIndex))
	{
		float limit_(0.f);
		UCurveFloat* stretchCurve = nullptr;
		if (bStretch)
		{
			switch (handAction)
			{
			case EOctoHandAction::MOVE_TO_ATTACH_POINT:
				stretchCurve = walkHandStretchCurve;
				break;
			case EOctoHandAction::ATTACHED:
				stretchCurve = walkHandStretchCurve;
				break;
			case EOctoHandAction::ATTACKS:
				stretchCurve = attackHandStretchCurve;
				break;
			default:
				break;
			}

			if (stretchCurve)
			{
				for (int i = 0; i < tentaclesArr[handIndex].constraintCompsArr.Num(); ++i)
				{
					limit_ = stretchCurve->GetFloatValue(static_cast<float>(i));
					tentaclesArr[handIndex].constraintCompsArr[i]->SetLinearXLimit(LCM_Limited, limit_);
					tentaclesArr[handIndex].constraintCompsArr[i]->SetLinearYLimit(LCM_Limited, limit_);
					tentaclesArr[handIndex].constraintCompsArr[i]->SetLinearZLimit(LCM_Limited, limit_);
				}
			}
		}
		else
		{
			for (int i = 0; i < tentaclesArr[handIndex].constraintCompsArr.Num(); ++i)
			{
				tentaclesArr[handIndex].constraintCompsArr[i]->SetLinearXLimit(LCM_Locked, limit_);
				tentaclesArr[handIndex].constraintCompsArr[i]->SetLinearYLimit(LCM_Locked, limit_);
				tentaclesArr[handIndex].constraintCompsArr[i]->SetLinearZLimit(LCM_Locked, limit_);
			}
		}
	}
}
