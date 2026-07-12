// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "MultiplayerSessionSubsystem.generated.h"

/**
 * 
 */
class FOnlineSessionSearch;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSessionSearchCompleted, bool, bWasSuccessful, int32, ResultCount);

UCLASS(BlueprintType)
class MULTIPLAYERARENA_API UMultiplayerSessionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Category = "Multiplayer|Sessions")
	void CreateSession(int32 NumPublicConnections = 4);

	UFUNCTION(BlueprintCallable, Category = "Multiplayer|Sessions")
	void FindSessions(int32 MaxSearchResults = 50);

	UFUNCTION(BlueprintCallable, Category = "Multiplayer|Sessions")
	void JoinSession(int32 SearchResultIndex = 0);

	UPROPERTY(BlueprintAssignable, Category = "Multiplayer|Sessions")
	FOnSessionSearchCompleted OnSessionSearchCompleted;
	
private:
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnFindSessionsComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(	FName SessionName,	EOnJoinSessionCompleteResult::Type Result);

	FDelegateHandle CreateSessionCompleteDelegateHandle;
	FDelegateHandle FindSessionsCompleteDelegateHandle;
	FDelegateHandle JoinSessionCompleteDelegateHandle;

	IOnlineSessionPtr SessionInterface;
	TSharedPtr<FOnlineSessionSearch> SessionSearch;
};
