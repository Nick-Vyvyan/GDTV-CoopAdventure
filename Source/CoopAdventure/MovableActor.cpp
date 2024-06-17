// Fill out your copyright notice in the Description page of Project Settings.


#include "MovableActor.h"
#include "Components/ArrowComponent.h"

// Sets default values
AMovableActor::AMovableActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetReplicateMovement(true);

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	SetRootComponent(RootComp);

	Point1 = CreateDefaultSubobject<UArrowComponent>(TEXT("Point1"));
	Point1->SetupAttachment(RootComp);
	Point1->SetRelativeLocation(FVector::Zero());

	Point2 = CreateDefaultSubobject<UArrowComponent>(TEXT("Point2"));
	Point2->SetupAttachment(RootComp);
	Point2->SetRelativeLocation(FVector(0, 0, 300.f));
	Point2->ArrowColor = FColor::Magenta;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComp);
	Mesh->SetIsReplicated(true);

	Transporter = CreateDefaultSubobject<UTransporter>(TEXT("Transporter"));

}

// Called when the game starts or when spawned
void AMovableActor::BeginPlay()
{
	Super::BeginPlay();

	FVector StartPoint = GetActorLocation() + Point1->GetRelativeLocation();
	FVector EndPoint = GetActorLocation() + Point2->GetRelativeLocation();

	Transporter->SetPoints(StartPoint, EndPoint);

}

// Called every frame
void AMovableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

