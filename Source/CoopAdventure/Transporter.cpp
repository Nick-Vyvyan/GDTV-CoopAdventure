// Fill out your copyright notice in the Description page of Project Settings.


#include "Transporter.h"

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



}


// Called every frame
void UTransporter::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


}


