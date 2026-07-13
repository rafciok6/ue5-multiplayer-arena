// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "ArenaGameState.generated.h"

/**
 * 
 */

//Store match state

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnArenaMatchStateChanged);

UCLASS()
class MULTIPLAYERARENA_API AArenaGameState : public AGameStateBase
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure, Category = "Arena")
	int32 GetRemainingMatchTime() const
	{
		return RemainingMatchTime;
	}

	UFUNCTION(BlueprintPure, Category = "Arena")
	bool IsMatchFinished() const
	{
		return bMatchFinished;
	}

	void SetRemainingMatchTime(int32 NewRemainingTime);
	void SetMatchFinished(bool bNewMatchFinished);

	UPROPERTY(BlueprintAssignable, Category = "Arena")
	FOnArenaMatchStateChanged OnMatchStateChanged;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UFUNCTION()
	void OnRep_RemainingMatchTime();

	UFUNCTION()
	void OnRep_MatchFinished();

	void NotifyMatchStateChanged();

	UPROPERTY(ReplicatedUsing = OnRep_RemainingMatchTime)
	int32 RemainingMatchTime = 0;

	UPROPERTY(ReplicatedUsing = OnRep_MatchFinished)
	bool bMatchFinished = false;
};
