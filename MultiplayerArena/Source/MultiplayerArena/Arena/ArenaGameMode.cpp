// Fill out your copyright notice in the Description page of Project Settings.


#include "ArenaGameMode.h"
#include "ArenaGameState.h"
#include "ArenaPlayerState.h"
#include "TimerManager.h"

AArenaGameMode::AArenaGameMode()
{
	GameStateClass = AArenaGameState::StaticClass();
	PlayerStateClass = AArenaPlayerState::StaticClass();
	bUseSeamlessTravel = true;
}

void AArenaGameMode::BeginPlay()
{
	Super::BeginPlay();

	ArenaGameState = GetGameState<AArenaGameState>();
	if (!IsValid(ArenaGameState))
	{
		UE_LOG(LogTemp, Error, TEXT("ArenaGameState is not available"));
		return;
	}

	RemainingMatchTime = MatchDuration;

	ArenaGameState->SetMatchFinished(false);
	ArenaGameState->SetRemainingMatchTime(RemainingMatchTime);

	GetWorldTimerManager().SetTimer(MatchTimerHandle,this, &AArenaGameMode::UpdateMatchTimer,1.0f,true);

	UE_LOG(
		LogTemp,
		Log,
		TEXT("Arena match started. Duration: %d seconds"),
		MatchDuration);
}

void AArenaGameMode::UpdateMatchTimer()
{
	if (RemainingMatchTime <= 0)
	{
		FinishRound();
		return;
	}

	--RemainingMatchTime;

	if (IsValid(ArenaGameState))
	{
		ArenaGameState->SetRemainingMatchTime(RemainingMatchTime);
	}

	if (RemainingMatchTime <= 0)
	{
		FinishRound();
	}
}

void AArenaGameMode::FinishRound()
{
	GetWorldTimerManager().ClearTimer(MatchTimerHandle);

	if (IsValid(ArenaGameState))
	{
		ArenaGameState->SetMatchFinished(true);
	}

	UE_LOG(LogTemp, Log, TEXT("Arena match finished"));
}