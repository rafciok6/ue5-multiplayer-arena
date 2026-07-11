// Fill out your copyright notice in the Description page of Project Settings.

#include "MultiplayerSessionSubsystem.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystemUtils.h"
#include "GameFramework/PlayerController.h"

void UMultiplayerSessionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	SessionInterface = Online::GetSessionInterface(GetWorld());

	if (!SessionInterface.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Session interface is not available"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Session interface initialized"));
}

void UMultiplayerSessionSubsystem::Deinitialize()
{
	if (SessionInterface.IsValid())
	{
		if (CreateSessionCompleteDelegateHandle.IsValid())
		{
			SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
		}

		if (FindSessionsCompleteDelegateHandle.IsValid())
		{
			SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);
		}
		
		if (JoinSessionCompleteDelegateHandle.IsValid())
		{
			SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);
		}
	}

	CreateSessionCompleteDelegateHandle.Reset();
	FindSessionsCompleteDelegateHandle.Reset();
	JoinSessionCompleteDelegateHandle.Reset();
	
	SessionSearch.Reset();
	SessionInterface.Reset();

	UE_LOG(LogTemp, Log, TEXT("MultiplayerSessionSubsystem deinitialized"));

	Super::Deinitialize();
}

void UMultiplayerSessionSubsystem::CreateSession(int32 NumPublicConnections)
{
	if (!SessionInterface.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot create session: session interface is invalid"));
		return;
	}

	if (SessionInterface->GetNamedSession(NAME_GameSession) != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot create session: session already exists"));
		return;
	}

	CreateSessionCompleteDelegateHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(
			FOnCreateSessionCompleteDelegate::CreateUObject( this, &UMultiplayerSessionSubsystem::OnCreateSessionComplete));

	FOnlineSessionSettings SessionSettings;
	SessionSettings.bIsLANMatch = true;
	SessionSettings.NumPublicConnections = FMath::Max(1, NumPublicConnections);
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.bAllowJoinInProgress = true;
	SessionSettings.bUsesPresence = false;
	SessionSettings.bAllowJoinViaPresence = false;
	SessionSettings.bIsDedicated = false;

	const bool bRequestStarted = SessionInterface->CreateSession(0, NAME_GameSession, SessionSettings);

	if (!bRequestStarted)
	{
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
		CreateSessionCompleteDelegateHandle.Reset();

		UE_LOG(LogTemp, Error, TEXT("Create session request could not be started"));
	}
}

void UMultiplayerSessionSubsystem::FindSessions(int32 MaxSearchResults)
{
	if (!SessionInterface.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot find sessions: session interface is invalid"));
		return;
	}

	if (FindSessionsCompleteDelegateHandle.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot find sessions: search is already in progress"));
		return;
	}

	SessionSearch = MakeShared<FOnlineSessionSearch>();
	SessionSearch->bIsLanQuery = true;
	SessionSearch->MaxSearchResults = FMath::Max(1, MaxSearchResults);

	FindSessionsCompleteDelegateHandle =
		SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(
			FOnFindSessionsCompleteDelegate::CreateUObject(
				this,
				&UMultiplayerSessionSubsystem::OnFindSessionsComplete));

	const bool bRequestStarted =
		SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());

	if (!bRequestStarted)
	{
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(
			FindSessionsCompleteDelegateHandle);

		FindSessionsCompleteDelegateHandle.Reset();
		SessionSearch.Reset();

		UE_LOG(LogTemp, Error, TEXT("Find sessions request could not be started"));
	}
}

void UMultiplayerSessionSubsystem::JoinSession(int32 SearchResultIndex)
{
	if (!SessionInterface.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot join session: session interface is invalid"));
		return;
	}

	if (!SessionSearch.IsValid() || !SessionSearch->SearchResults.IsValidIndex(SearchResultIndex))
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("Cannot join session: invalid search result index %d"),
			SearchResultIndex);

		return;
	}

	if (JoinSessionCompleteDelegateHandle.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot join session: request already in progress"));
		return;
	}

	JoinSessionCompleteDelegateHandle =	SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(
			FOnJoinSessionCompleteDelegate::CreateUObject(
				this,
				&UMultiplayerSessionSubsystem::OnJoinSessionComplete));

	const bool bRequestStarted = SessionInterface->JoinSession(
		0,
		NAME_GameSession,
		SessionSearch->SearchResults[SearchResultIndex]);

	if (!bRequestStarted)
	{
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);
		JoinSessionCompleteDelegateHandle.Reset();

		UE_LOG(LogTemp, Error, TEXT("Failed to start join session request"));
	}
}

void UMultiplayerSessionSubsystem::OnCreateSessionComplete(	FName SessionName,	bool bWasSuccessful)
{
	if (SessionInterface.IsValid())
	{
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
	}

	CreateSessionCompleteDelegateHandle.Reset();

	if (!bWasSuccessful)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create session: %s"), *SessionName.ToString());
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Session created successfully: %s"), *SessionName.ToString());

	UWorld* World = GetWorld();

	if (World == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot start listen server: world is invalid"));
		return;
	}

	World->ServerTravel(TEXT("/Game/Maps/Lvl_Lobby?listen"));
}

void UMultiplayerSessionSubsystem::OnFindSessionsComplete(bool bWasSuccessful)
{
	if (SessionInterface.IsValid())
	{
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);
	}

	FindSessionsCompleteDelegateHandle.Reset();

	if (!bWasSuccessful || !SessionSearch.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to find sessions"));
		return;
	}

	const int32 ResultCount = SessionSearch->SearchResults.Num();

	UE_LOG(LogTemp, Log, TEXT("Session search completed. Results: %d"), ResultCount);

	for (int32 Index = 0; Index < ResultCount; ++Index)
	{
		const FOnlineSessionSearchResult& Result = SessionSearch->SearchResults[Index];

		UE_LOG(
			LogTemp,
			Log,
			TEXT("Session %d: Owner=%s, Ping=%d ms, OpenConnections=%d"),
			Index,
			*Result.Session.OwningUserName,
			Result.PingInMs,
			Result.Session.NumOpenPublicConnections);
	}
}

void UMultiplayerSessionSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (SessionInterface.IsValid() && JoinSessionCompleteDelegateHandle.IsValid())
	{
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);
	}

	JoinSessionCompleteDelegateHandle.Reset();

	if (Result != EOnJoinSessionCompleteResult::Success)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("Failed to join session. Result: %d"),
			static_cast<int32>(Result));

		return;
	}

	if (!SessionInterface.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Session interface became invalid after joining"));
		return;
	}

	FString ConnectString;

	if (!SessionInterface->GetResolvedConnectString(SessionName, ConnectString))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to resolve session connect string"));
		return;
	}

	UWorld* World = GetWorld();

	if (World == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot travel to session: world is invalid"));
		return;
	}

	APlayerController* PlayerController = World->GetFirstPlayerController();

	if (PlayerController == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot travel to session: player controller is invalid"));
		return;
	}

	UE_LOG(
		LogTemp,
		Log,
		TEXT("Joined session successfully. Travelling to %s"),
		*ConnectString);

	PlayerController->ClientTravel(ConnectString, TRAVEL_Absolute);
}
