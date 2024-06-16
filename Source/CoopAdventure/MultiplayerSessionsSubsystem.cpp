// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSubsystem.h"
#include "Online/OnlineSessionNames.h"



void static PrintDebugString(const FString& Message, FColor Color = FColor::Cyan)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5, Color, Message);
	}
}

UMultiplayerSessionsSubsystem::UMultiplayerSessionsSubsystem()
{
	//PrintDebugString("MSS Constructor");
	MySessionName = FName("Co-op Adventure Session Name");
}

void UMultiplayerSessionsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	//PrintString("MSS_Initialize");

	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem)
	{
		FString SubsystemName = OnlineSubsystem->GetSubsystemName().ToString();
		PrintDebugString(SubsystemName);

		SessionInterface = OnlineSubsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			//PrintDebugString("Session Interface is Valid!");
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UMultiplayerSessionsSubsystem::OnCreateSessionComplete);
			SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UMultiplayerSessionsSubsystem::OnDestroySessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UMultiplayerSessionsSubsystem::OnFindSessionsComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UMultiplayerSessionsSubsystem::OnJoinSessionComplete);
		}
	}
}

void UMultiplayerSessionsSubsystem::Deinitialize()
{
	//UE_LOG(LogTemp, Warning, TEXT("MSS_Deinitialize"));
}

void UMultiplayerSessionsSubsystem::CreateServer(FString ServerName)
{
	PrintDebugString("CreateServer: " + ServerName);

	if (ServerName.IsEmpty())
	{
		PrintDebugString("ServerName cannot be empty!", FColor::Red);
		ServerCreateDel.Broadcast(false);
		return;
	}

	FNamedOnlineSession* ExistingSession = SessionInterface->GetNamedSession(MySessionName);
	if (ExistingSession)
	{
		FString Msg = FString::Printf(TEXT("Session %s Already Exists, Destroying..."), *MySessionName.ToString());
		PrintDebugString(Msg, FColor::Red);
		bCreateServerAfterDestroy = true;
		DestroyServerName = ServerName;
		SessionInterface->DestroySession(MySessionName);
		return;
	}

	FOnlineSessionSettings SessionSettings;
	SessionSettings.bAllowJoinInProgress = true;
	SessionSettings.bIsDedicated = false;
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.NumPublicConnections = 2;
	SessionSettings.bUseLobbiesIfAvailable = true;
	SessionSettings.bUsesPresence = true;
	SessionSettings.bAllowJoinViaPresence = true;
	bool bIsLan = false;
	if (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL")
	{
		bIsLan = true;
	}
	SessionSettings.bIsLANMatch = bIsLan;

	SessionSettings.Set(FName("SERVER_NAME"), ServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	SessionInterface->CreateSession(0, MySessionName, SessionSettings);
}

void UMultiplayerSessionsSubsystem::FindServer(FString ServerName)
{
	//PrintDebugString("FindServer: " + ServerName);
	if (ServerName.IsEmpty())
	{
		PrintDebugString("ServerName cannot be empty!", FColor::Red);
		ServerJoinDel.Broadcast(false);
		return;
	}

	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	bool bIsLan = false;
	if (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL")
	{
		bIsLan = true;
	}
	SessionSearch->bIsLanQuery = bIsLan;
	SessionSearch->MaxSearchResults = 9999;
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	ServerNameToFind = ServerName;

	SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
}

void UMultiplayerSessionsSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	PrintDebugString(FString::Printf(TEXT("OnCreateSessionComplete: %d"), bWasSuccessful));

	ServerCreateDel.Broadcast(bWasSuccessful);

	if (bWasSuccessful)
	{
		GetWorld()->ServerTravel("/Game/ThirdPerson/Maps/ThirdPersonMap?listen");
	}
}

void UMultiplayerSessionsSubsystem::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	PrintDebugString("DestroySessionComplete");

	if (bWasSuccessful && bCreateServerAfterDestroy)
	{
		bCreateServerAfterDestroy = false;
		CreateServer(DestroyServerName);
	}
}

void UMultiplayerSessionsSubsystem::OnFindSessionsComplete(bool bWasSuccessful)
{
	if (!bWasSuccessful)
	{
		ServerJoinDel.Broadcast(false);
		return;
	}
	if (ServerNameToFind.IsEmpty())
	{
		ServerJoinDel.Broadcast(false);
		return;
	}

	TArray<FOnlineSessionSearchResult> Results = SessionSearch->SearchResults;
	if (Results.Num() > 0)
	{
		FString Msg = FString::Printf(TEXT("Sessions Found: %d"), Results.Num());
		PrintDebugString(Msg);

		FOnlineSessionSearchResult* CorrectResult = 0;

		for (FOnlineSessionSearchResult Result : Results)
		{
			if (Result.IsValid())
			{
				FString ServerName = "No_Name";
				Result.Session.SessionSettings.Get(FName("SERVER_NAME"), ServerName);

				if (ServerName.Equals(ServerNameToFind))
				{
					CorrectResult = &Result;
					FString Msg2 = FString::Printf(TEXT("Found Server with name: %s"), *ServerName);
					PrintDebugString(Msg2, FColor::Green);
					break;
				}
			}
		}

		if (CorrectResult)
		{
			SessionInterface->JoinSession(0, MySessionName, *CorrectResult);
		}
		else
		{
			PrintDebugString("Could Not Find Server: " + ServerNameToFind, FColor::Red);
			ServerNameToFind = "";
			ServerJoinDel.Broadcast(false);
		}
	}
	else
	{
		PrintDebugString("Zero Sessions Found", FColor::Red);
		ServerJoinDel.Broadcast(false);
	}


}

void UMultiplayerSessionsSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	ServerJoinDel.Broadcast(Result == EOnJoinSessionCompleteResult::Success);

	if (Result == EOnJoinSessionCompleteResult::Success)
	{
		FString Msg = FString::Printf(TEXT("Successfully Joined Session %s"), *SessionName.ToString());
		PrintDebugString(Msg, FColor::Green);

		FString Address = "";
		bool bSuccess = SessionInterface->GetResolvedConnectString(MySessionName, Address);
		if (bSuccess)
		{
			PrintDebugString(FString::Printf(TEXT("Address: %s"), *Address));

			APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController(GetWorld());
			if (PlayerController)
			{
				PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
			}
		}
		else
		{
			PrintDebugString("GetResolvedConnectString returned false!", FColor::Red);
		}
	}
	else
	{
		PrintDebugString("OnJoinSessionComplete failed", FColor::Red);
	}
}

