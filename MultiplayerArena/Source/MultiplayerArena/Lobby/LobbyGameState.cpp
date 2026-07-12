// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameState.h"
#include "LobbyPlayerState.h"

int32 ALobbyGameState::GetReadyPlayerCount() const
{
	int32 ReadyPlayerCount = 0;

	for (const APlayerState* PlayerState : PlayerArray)
	{
		const ALobbyPlayerState* LobbyPlayerState = Cast<ALobbyPlayerState>(PlayerState);

		if (LobbyPlayerState != nullptr && LobbyPlayerState->IsReady())
		{
			++ReadyPlayerCount;
		}
	}

	return ReadyPlayerCount;
}

bool ALobbyGameState::AreAllPlayersReady() const
{
	if (PlayerArray.IsEmpty())
	{
		return false;
	}

	return GetReadyPlayerCount() == PlayerArray.Num();
}

void ALobbyGameState::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);

	OnLobbyPlayersChanged.Broadcast();
}

void ALobbyGameState::RemovePlayerState(APlayerState* PlayerState)
{
	Super::RemovePlayerState(PlayerState);

	OnLobbyPlayersChanged.Broadcast();
}

void ALobbyGameState::NotifyLobbyPlayersChanged()
{
	OnLobbyPlayersChanged.Broadcast();
}