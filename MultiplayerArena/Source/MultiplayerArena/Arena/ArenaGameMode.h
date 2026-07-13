// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ArenaGameMode.generated.h"

/**
 * 
 */
class AArenaGameState;
class AArenaCharacter;

UCLASS()
class MULTIPLAYERARENA_API AArenaGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AArenaGameMode();

	void HandlePlayerDeath(AArenaCharacter* VictimCharacter, AController* KillerController);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Arena", meta = (ClampMin = "10"))
	int32 MatchDuration = 120;

	UPROPERTY(EditDefaultsOnly, Category = "Arena", meta = (ClampMin = "0.0"))
	float RespawnDelay = 3.0f;

	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

	UPROPERTY(EditDefaultsOnly,Category = "Arena|Round End",meta = (ClampMin = "1.0"))
	float ResultsDisplayDuration = 8.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Arena|Round End")
	FString LobbyMapPath = TEXT("/Game/Maps/Lvl_Lobby");
	
private:
	void UpdateMatchTimer();
	void FinishRound();
	void RespawnPlayer(AController* PlayerController);
	void CalculateMatchResult(FString& OutWinnerName,int32& OutWinningScore) const;
	void ReturnToLobby();
	
	UPROPERTY()
	TObjectPtr<AArenaGameState> ArenaGameState;
	
	FTimerHandle MatchTimerHandle;
	FTimerHandle ReturnToLobbyTimerHandle;
	int32 RemainingMatchTime = 0;
};


