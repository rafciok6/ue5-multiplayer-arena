// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "LobbyPlayerState.h"
#include "LobbyGameState.h"
#include "LobbyPlayerController.h"
#include "Engine/World.h"

ALobbyGameMode::ALobbyGameMode()
{
	GameStateClass = ALobbyGameState::StaticClass();
	PlayerStateClass = ALobbyPlayerState::StaticClass();
	PlayerControllerClass = ALobbyPlayerController::StaticClass();

	DefaultPawnClass = nullptr;
	bUseSeamlessTravel = true;
}

void ALobbyGameMode::TryStartMatch(APlayerController* RequestingController)
{
	if (RequestingController == nullptr || !RequestingController->IsLocalController())
	{
		UE_LOG(LogTemp, Warning, TEXT("Start match rejected: requester is not the host"));
		return;
	}

	ALobbyGameState* LobbyGameState = GetGameState<ALobbyGameState>();

	if (LobbyGameState == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Start match failed: invalid lobby game state"));
		return;
	}

	const int32 PlayerCount = LobbyGameState->PlayerArray.Num();

	if (PlayerCount < MinimumPlayersToStart)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("Start match rejected: requires at least %d players, currently %d"),
			MinimumPlayersToStart,
			PlayerCount);

		return;
	}

	if (!LobbyGameState->AreAllPlayersReady())
	{
		UE_LOG(LogTemp, Warning, TEXT("Start match rejected: not all players are ready"));
		return;
	}

	if (ArenaMapPath.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("Start match failed: arena map path is empty"));
		return;
	}

	UWorld* World = GetWorld();

	if (World == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Start match failed: world is invalid"));
		return;
	}

	UE_LOG(
		LogTemp,
		Log,
		TEXT("All %d players are ready. Starting match on %s"),
		PlayerCount,
		*ArenaMapPath);

	World->ServerTravel(ArenaMapPath);
}