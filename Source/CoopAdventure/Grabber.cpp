// Fill out your copyright notice in the Description page of Project Settings.


#include "Grabber.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

}


// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	PhysicsHandle = GetPhysicsHandle();
	if (!PhysicsHandle)
	{
		return;
	}

	UPrimitiveComponent* GrabbedComponent = PhysicsHandle->GetGrabbedComponent();
	if (!GrabbedComponent)
	{
		return;
	}

	FVector TargetLocation = GetComponentLocation() + GetForwardVector() * HoldDistance;
	PhysicsHandle->SetTargetLocationAndRotation(TargetLocation, GetComponentRotation());
}

void UGrabber::Grab()
{
	if (!PhysicsHandle)
	{
		return;
	}
	FHitResult HitResult;
	if (GetGrabbableInReach(HitResult))
	{
		UPrimitiveComponent* HitComponent = HitResult.GetComponent();
		HitComponent->WakeAllRigidBodies();
		HitResult.GetActor()->Tags.Add("Grabbed");
		HitResult.GetComponent()->SetSimulatePhysics(true);
		HitResult.GetActor()->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

		PhysicsHandle->GrabComponentAtLocationWithRotation(
			HitComponent,
			NAME_None,
			HitResult.ImpactPoint,
			GetComponentRotation()
		);
	}
}

void UGrabber::Release()
{
	if (!PhysicsHandle)
	{
		return;
	}

	if (!PhysicsHandle->GetGrabbedComponent())
	{
		return;
	}

	PhysicsHandle->GetGrabbedComponent()->GetOwner()->Tags.Remove("Grabbed");
	PhysicsHandle->GetGrabbedComponent()->WakeRigidBody();
	PhysicsHandle->ReleaseComponent();
}

UPhysicsHandleComponent* UGrabber::GetPhysicsHandle() const
{
	UPhysicsHandleComponent* Res = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (!Res)
	{
		UE_LOG(LogTemp, Error, TEXT("Grabber Requires a Physics Handle Component!"));
	}
	return Res;
}

bool UGrabber::GetGrabbableInReach(FHitResult& OutHit) const
{
	FVector Start = GetComponentLocation();
	FVector End = Start + GetForwardVector() * MaxGrabDistance;
	FCollisionShape Sphere = FCollisionShape::MakeSphere(GrabRadius);

	bool bHasHit = GetWorld()->SweepSingleByChannel(
		OutHit,
		Start,
		End,
		FQuat::Identity,
		ECC_GameTraceChannel1,
		Sphere
	);
	bool bIsAlreadyGrabbed = OutHit.GetActor()->Tags.Contains(TEXT("Grabbed"));

	return bHasHit && !bIsAlreadyGrabbed;
}

