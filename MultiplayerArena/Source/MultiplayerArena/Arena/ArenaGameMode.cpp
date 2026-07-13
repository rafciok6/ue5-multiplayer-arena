// Fill out your copyright notice in the Description page of Project Settings.


#include "ArenaGameMode.h"
#include "ArenaGameState.h"
#include "ArenaPlayerState.h"
#include "TimerManager.h"
#include "ArenaCharacter.h"
#include "../Player/MultiplayerArenaPlayerState.h"

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

void AArenaGameMode::HandlePlayerDeath(AArenaCharacter* VictimCharacter, AController* KillerController)
{
	if (!IsValid(VictimCharacter))
	{
		return;
	}

	if (IsValid(ArenaGameState) && ArenaGameState->IsMatchFinished())
	{
		return;
	}

	AController* VictimController = VictimCharacter->GetController();

	if (!IsValid(VictimController))
	{
		return;
	}

	AMultiplayerArenaPlayerState* VictimPlayerState = VictimController->GetPlayerState<AMultiplayerArenaPlayerState>();

	if (IsValid(VictimPlayerState))
	{
		VictimPlayerState->AddDeath();
	}

	if (IsValid(KillerController) && KillerController != VictimController)
	{
		if (AMultiplayerArenaPlayerState* KillerPlayerState = KillerController->GetPlayerState<AMultiplayerArenaPlayerState>())
		{
			KillerPlayerState->AddKill();
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Player %s died. Respawning in %.1f seconds"),
		IsValid(VictimPlayerState)
			? *VictimPlayerState->GetPlayerName()
			: TEXT("Unknown"),
		RespawnDelay);

	VictimController->UnPossess();
	VictimCharacter->Destroy();

	if (RespawnDelay <= 0.0f)
	{
		RespawnPlayer(VictimController);
		return;
	}

	FTimerDelegate RespawnDelegate;
	RespawnDelegate.BindUObject(this, &AArenaGameMode::RespawnPlayer, VictimController);

	FTimerHandle RespawnTimerHandle;
	GetWorldTimerManager().SetTimer(RespawnTimerHandle, RespawnDelegate, RespawnDelay,false);
}

void AArenaGameMode::RespawnPlayer(AController* PlayerController)
{
	if (!IsValid(PlayerController))
	{
		return;
	}

	if (IsValid(ArenaGameState) && ArenaGameState->IsMatchFinished())
	{
		return;
	}

	RestartPlayer(PlayerController);

	UE_LOG(
		LogTemp,
		Log,
		TEXT("Player respawned"));
}