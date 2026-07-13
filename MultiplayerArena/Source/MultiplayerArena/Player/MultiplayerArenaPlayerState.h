// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "MultiplayerArenaPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERARENA_API AMultiplayerArenaPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Player Stats")
	int32 GetKills() const { return Kills; }

	UFUNCTION(BlueprintPure, Category = "Player Stats")
	int32 GetDeaths() const { return Deaths; }

	void AddKill();
	void AddDeath();
	void ResetMatchStats();

	virtual void CopyProperties(APlayerState* PlayerState) override;
	virtual void OverrideWith(APlayerState* PlayerState) override;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Player Stats")
	int32 Kills = 0;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Player Stats")
	int32 Deaths = 0;
};
