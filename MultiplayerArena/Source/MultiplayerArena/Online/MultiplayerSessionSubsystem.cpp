// Fill out your copyright notice in the Description page of Project Settings.

#include "MultiplayerSessionSubsystem.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystemUtils.h"

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
	if (SessionInterface.IsValid() && CreateSessionCompleteDelegateHandle.IsValid())
	{
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
	}

	CreateSessionCompleteDelegateHandle.Reset();
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
