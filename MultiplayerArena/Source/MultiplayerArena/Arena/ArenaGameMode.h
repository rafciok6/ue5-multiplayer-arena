// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ArenaGameMode.generated.h"

/**
 * 
 */
class AArenaGameState;

UCLASS()
class MULTIPLAYERARENA_API AArenaGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AArenaGameMode();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Arena", meta = (ClampMin = "10"))
	int32 MatchDuration = 120;

private:
	void UpdateMatchTimer();
	
	void FinishRound();
	
	UPROPERTY()
	TObjectPtr<AArenaGameState> ArenaGameState;
	
	FTimerHandle MatchTimerHandle;
	int32 RemainingMatchTime = 0;
};
