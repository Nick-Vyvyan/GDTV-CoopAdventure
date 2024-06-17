// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PressurePlate.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPressurePlateOnActivated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPressurePlateOnDeactivated);

UCLASS()
class COOPADVENTURE_API APressurePlate : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APressurePlate();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	USceneComponent* RootComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* TriggerMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bActivated;

	UPROPERTY(BlueprintAssignable)
	FPressurePlateOnActivated OnActivated;

	UPROPERTY(BlueprintAssignable)
	FPressurePlateOnDeactivated OnDeactivated;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UTransporter* Transporter;

private:
	void TriggerActivation();

};
