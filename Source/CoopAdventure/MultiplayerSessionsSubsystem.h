// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "MultiplayerSessionsSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FServerCreateDelegate, bool, bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FServerJoinDelegate, bool, bWasSuccessful);

/**
 *
 */
UCLASS()
class COOPADVENTURE_API UMultiplayerSessionsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UMultiplayerSessionsSubsystem();

	// Begin USubsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	// End USubsystem

	IOnlineSessionPtr SessionInterface;

	UFUNCTION(BlueprintCallable)
	void CreateServer(FString ServerName);

	UFUNCTION(BlueprintCallable)
	void FindServer(FString ServerName);

	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);
	void OnFindSessionsComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	bool bCreateServerAfterDestroy = false;
	FString DestroyServerName = "";
	FString ServerNameToFind = "";
	FName MySessionName;

	TSharedPtr<FOnlineSessionSearch> SessionSearch;

	UPROPERTY(BlueprintAssignable)
	FServerCreateDelegate ServerCreateDel;

	UPROPERTY(BlueprintAssignable)
	FServerJoinDelegate ServerJoinDel;

	UPROPERTY(BlueprintReadWrite)
	FString GameMapPath;
};
