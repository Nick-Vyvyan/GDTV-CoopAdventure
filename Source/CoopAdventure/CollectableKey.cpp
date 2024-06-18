// Fill out your copyright notice in the Description page of Project Settings.


#include "CollectableKey.h"
#include "Components/CapsuleComponent.h"
#include "Net/UnrealNetwork.h"
#include "CoopAdventureCharacter.h"
#include "Components/AudioComponent.h"
#include "CollectableKeyHolder.h"

// Sets default values
ACollectableKey::ACollectableKey()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetReplicateMovement(true);

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	SetRootComponent(RootComp);

	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	Capsule->SetupAttachment(RootComp);
	Capsule->SetIsReplicated(true);
	Capsule->SetCollisionProfileName("OverlapAllDynamic");
	Capsule->SetCapsuleHalfHeight(150.f);
	Capsule->SetCapsuleRadius(100.f);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComp);
	Mesh->SetIsReplicated(true);
	Mesh->SetCollisionProfileName(FName("OverlapAllDynamic"));

	CollectAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("CollectAudio"));
	CollectAudio->SetupAttachment(RootComp);
	CollectAudio->SetAutoActivate(false);

	RotationSpeed = 100;

}

void ACollectableKey::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACollectableKey, bIsCollected);
}

void ACollectableKey::OnRep_IsCollected()
{
	if (HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("OnRep_IsCollected called from server"));
		if (bIsCollected)
		{
			OnCollected.Broadcast();
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("OnRep_IsCollected called from client"));
	}

	Mesh->SetVisibility(!bIsCollected);

	CollectAudio->Play();

	if (bIsCollected && KeyHolder)
	{
		KeyHolder->ActivateKeyMesh();
	}
}


// Called when the game starts or when spawned
void ACollectableKey::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ACollectableKey::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Rotate Static Mesh
	Mesh->AddRelativeRotation(FRotator(0, RotationSpeed * DeltaTime, 0));

	if (HasAuthority())
	{
		TArray<AActor*> OverlappingActors;
		Capsule->GetOverlappingActors(OverlappingActors, ACoopAdventureCharacter::StaticClass());

		if (!OverlappingActors.IsEmpty() && !bIsCollected)
		{
			bIsCollected = true;
			OnRep_IsCollected();
		}
	}

}


