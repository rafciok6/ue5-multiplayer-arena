// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "LobbyGameState.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLobbyPlayersChanged);

UCLASS()
class MULTIPLAYERARENA_API ALobbyGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	virtual void AddPlayerState(APlayerState* PlayerState) override;
	virtual void RemovePlayerState(APlayerState* PlayerState) override;
	
	UFUNCTION(BlueprintPure, Category = "Lobby")
	int32 GetReadyPlayerCount() const;

	UFUNCTION(BlueprintPure, Category = "Lobby")
	bool AreAllPlayersReady() const;

	UPROPERTY(BlueprintAssignable, Category = "Lobby")
	FOnLobbyPlayersChanged OnLobbyPlayersChanged;

	void NotifyLobbyPlayersChanged();
};
