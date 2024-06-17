// Fill out your copyright notice in the Description page of Project Settings.


#include "PressurePlate.h"

// Sets default values
APressurePlate::APressurePlate()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetReplicateMovement(true);

	bActivated = false;

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	SetRootComponent(RootComp);

	TriggerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TriggerMesh"));
	TriggerMesh->SetupAttachment(RootComp);
	TriggerMesh->SetIsReplicated(true);
	ConstructorHelpers::FObjectFinder<UStaticMesh> TriggerMeshAsset =
		ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("/Game/StarterContent/Shapes/Shape_Cylinder.Shape_Cylinder"));

	if (TriggerMeshAsset.Succeeded())
	{
		TriggerMesh->SetStaticMesh(TriggerMeshAsset.Object);
		TriggerMesh->SetRelativeScale3D(FVector(3, 3, 0.2f));
		TriggerMesh->SetRelativeLocation(FVector(0, 0, 10.f));
	}

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComp);
	Mesh->SetIsReplicated(true);
	ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset =
		ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("/Game/Stylized_Egypt/Meshes/building/SM_building_part_08.SM_building_part_08"));

	if (MeshAsset.Succeeded())
	{
		Mesh->SetStaticMesh(TriggerMeshAsset.Object);
		Mesh->SetRelativeScale3D(FVector(4.f, 4.f, 0.5f));
		Mesh->SetRelativeLocation(FVector(0.f, 0.f, 7.2f));
	}


}

// Called when the game starts or when spawned
void APressurePlate::BeginPlay()
{
	Super::BeginPlay();

	TriggerMesh->SetVisibility(false);
	TriggerMesh->SetCollisionProfileName(FName("OverlapAll"));

}

// Called every frame
void APressurePlate::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TriggerActivation();


}

void APressurePlate::TriggerActivation()
{
	if (HasAuthority())
	{
		TArray<AActor*> OverlappingActors;
		AActor* TriggerActor = 0;
		TriggerMesh->GetOverlappingActors(OverlappingActors);

		for (int i = 0; i < OverlappingActors.Num(); i++)
		{
			AActor* A = OverlappingActors[i];
			if (A->ActorHasTag("TriggerActor"))
			{
				TriggerActor = A;
				break;
			}

			//FString Msg = FString::Printf(TEXT("Overlapping: %s"), *A->GetName());
			//GEngine->AddOnScreenDebugMessage(-1, 1, FColor::White, Msg);
		}

		if (TriggerActor)
		{
			if (!bActivated)
			{
				bActivated = true;
				GEngine->AddOnScreenDebugMessage(-1, 1, FColor::White, TEXT("Activated"));
			}
		}
		else
		{
			if (bActivated)
			{
				bActivated = false;
				GEngine->AddOnScreenDebugMessage(-1, 1, FColor::White, TEXT("Deactivated"));
			}
		}
	}
}

