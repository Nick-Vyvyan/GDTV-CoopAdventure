// Fill out your copyright notice in the Description page of Project Settings.


#include "Transporter.h"
#include "PressurePlate.h"
#include "CollectableKey.h"

// Sets default values for this component's properties
UTransporter::UTransporter()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);

	MoveTime = 3.f;
	ActivatedTriggerCount = 0;
	bArePointsSet = false;
	StartPoint = FVector::Zero();
	EndPoint = FVector::Zero();



}

void UTransporter::SetPoints(FVector Point1, FVector Point2)
{
	if (Point1.Equals(Point2))
	{
		return;
	}

	StartPoint = Point1;
	EndPoint = Point2;
	bArePointsSet = true;
}


// Called when the game starts
void UTransporter::BeginPlay()
{
	Super::BeginPlay();

	if (bOwnerIsTriggerActor)
	{
		TriggerActors.Add(GetOwner());
	}

	for (AActor* TA : TriggerActors)
	{
		APressurePlate* PressurePlateActor = Cast<APressurePlate>(TA);
		if (PressurePlateActor)
		{
			PressurePlateActor->OnActivated.AddDynamic(this, &UTransporter::OnTriggerActorActivated);
			PressurePlateActor->OnDeactivated.AddDynamic(this, &UTransporter::OnTriggerActorDeactivated);
			continue;
		}

		ACollectableKey* KeyActor = Cast<ACollectableKey>(TA);
		if (KeyActor)
		{
			KeyActor->OnCollected.AddDynamic(this, &UTransporter::OnTriggerActorActivated);
		}
	}

}

void UTransporter::OnTriggerActorActivated()
{
	ActivatedTriggerCount++;
	//FString Msg = FString::Printf(TEXT("Transporter Activated: %d"), ActivatedTriggerCount);
	//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::White, Msg);
}

void UTransporter::OnTriggerActorDeactivated()
{
	ActivatedTriggerCount--;
	//FString Msg = FString::Printf(TEXT("Transporter Deactivated: %d"), ActivatedTriggerCount);
	//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::White, Msg);
}

// Called every frame
void UTransporter::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateTriggerActorsTriggered();

	MoveOwner(DeltaTime);
}

void UTransporter::UpdateTriggerActorsTriggered()
{
	if (TriggerActors.Num() > 0)
	{
		bAllTriggerActorsTriggered = (ActivatedTriggerCount >= TriggerActors.Num());
		if (bAllTriggerActorsTriggered)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 1, FColor::White, FString("All Trigger Actors Triggered"));
		}
	}
}

void UTransporter::MoveOwner(float DeltaTime)
{
	AActor* MyOwner = GetOwner();

	if (MyOwner && MyOwner->HasAuthority() && bArePointsSet)
	{
		FVector CurrentLocation = MyOwner->GetActorLocation();
		float Speed = FVector::Dist(StartPoint, EndPoint) / MoveTime;

		FVector TargetLocation = bAllTriggerActorsTriggered ? EndPoint : StartPoint;

		if (!CurrentLocation.Equals(TargetLocation))
		{
			FVector NewLocation = FMath::VInterpConstantTo(CurrentLocation, TargetLocation, DeltaTime, Speed);
			MyOwner->SetActorLocation(NewLocation);
		}
	}
}


