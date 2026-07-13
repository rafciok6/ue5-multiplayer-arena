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

private:
	void UpdateMatchTimer();
	
	void FinishRound();

	void RespawnPlayer(AController* PlayerController);
	
	UPROPERTY()
	TObjectPtr<AArenaGameState> ArenaGameState;
	
	FTimerHandle MatchTimerHandle;
	int32 RemainingMatchTime = 0;
};
