// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerArenaGameInstance.h"

void UMultiplayerArenaGameInstance::SaveLastRoundResult(const FString& WinnerName, int32 WinningScore)
{
	const int32 ClampedScore =FMath::Max(0, WinningScore);

	if (bHasLastRoundResult &&LastRoundWinnerName == WinnerName && LastRoundWinningScore == ClampedScore)
	{
		return;
	}

	LastRoundWinnerName = WinnerName;
	LastRoundWinningScore = ClampedScore;
	bHasLastRoundResult = true;

	UE_LOG(LogTemp, Log,
		TEXT("Last round result saved: %s, Score: %d"),
		*LastRoundWinnerName,
		LastRoundWinningScore);
}

void UMultiplayerArenaGameInstance::ClearLastRoundResult()
{
	bHasLastRoundResult = false;
	LastRoundWinnerName.Empty();
	LastRoundWinningScore = 0;
}