// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LobbyGameMode.generated.h"

/**
 * 
 */
class APlayerController;

UCLASS()
class MULTIPLAYERARENA_API ALobbyGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ALobbyGameMode();

	void TryStartMatch(APlayerController* RequestingController);

private:
	void StartArenaTravel();

	FTimerHandle StartMatchTimerHandle;

	bool bMatchStartInProgress = false;

	UPROPERTY(EditDefaultsOnly, Category = "Lobby", meta = (ClampMin = "1"))
	int32 MinimumPlayersToStart = 2;

	UPROPERTY(EditDefaultsOnly, Category = "Lobby")
	FString ArenaMapPath = TEXT("/Game/Maps/Lvl_Arena");

	UPROPERTY(EditDefaultsOnly, Category = "Lobby", meta = (ClampMin = "0.1"))
	float LoadingScreenDisplayTime = 0.3f;
};
