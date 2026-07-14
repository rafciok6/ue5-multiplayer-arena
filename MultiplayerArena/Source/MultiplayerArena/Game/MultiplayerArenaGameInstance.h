// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MultiplayerArenaGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERARENA_API UMultiplayerArenaGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	void SaveLastRoundResult(const FString& WinnerName, int32 WinningScore);

	UFUNCTION(BlueprintCallable, Category = "Round Result")
	void ClearLastRoundResult();

	UFUNCTION(BlueprintPure, Category = "Round Result")
	bool HasLastRoundResult() const
	{
		return bHasLastRoundResult;
	}

	UFUNCTION(BlueprintPure, Category = "Round Result")
	FString GetLastRoundWinnerName() const
	{
		return LastRoundWinnerName;
	}

	UFUNCTION(BlueprintPure, Category = "Round Result")
	int32 GetLastRoundWinningScore() const
	{
		return LastRoundWinningScore;
	}

private:
	UPROPERTY()
	bool bHasLastRoundResult = false;

	UPROPERTY()
	FString LastRoundWinnerName;

	UPROPERTY()
	int32 LastRoundWinningScore = 0;
};