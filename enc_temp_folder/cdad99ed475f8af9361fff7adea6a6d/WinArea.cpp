// Fill out your copyright notice in the Description page of Project Settings.


#include "WinArea.h"
#include "Components/BoxComponent.h"
#include "CoopAdventureCharacter.h"
#include "CoopAdventureGameMode.h"

// Sets default values
AWinArea::AWinArea()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	WinAreaBox = CreateDefaultSubobject<UBoxComponent>(TEXT("WinAreaBox"));
	SetRootComponent(WinAreaBox);

	bWinCondition = false;
}

// Called when the game starts or when spawned
void AWinArea::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWinArea::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority())
	{
		if (!bWinCondition)
		{
			TArray<AActor*> OverlappingActors;
			WinAreaBox->GetOverlappingActors(OverlappingActors, ACoopAdventureCharacter::StaticClass());

			AGameModeBase* GameMode = GetWorld()->GetAuthGameMode();
			if (GameMode)
			{
				int NumPlayers = GameMode->GetNumPlayers();
				bWinCondition = (OverlappingActors.Num() == NumPlayers);
				if (bWinCondition)
				{
					UE_LOG(LogTemp, Warning, TEXT("Game Over: WIN!"));
				}
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Cannot Find Game Mode!!! (May not have Authority?)"));
			}
		}
	}
}

