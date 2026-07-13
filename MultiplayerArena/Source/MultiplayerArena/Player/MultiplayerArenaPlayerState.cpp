// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerArenaPlayerState.h"
#include "Net/UnrealNetwork.h"

void AMultiplayerArenaPlayerState::AddKill()
{
	if (!HasAuthority())
	{
		return;
	}

	++Kills;
	SetScore(GetScore() + 1.0f);
	NotifyStatsChanged();
	ForceNetUpdate();
}

void AMultiplayerArenaPlayerState::AddDeath()
{
	if (!HasAuthority())
	{
		return;
	}

	++Deaths;
	NotifyStatsChanged();
	ForceNetUpdate();
}

void AMultiplayerArenaPlayerState::ResetMatchStats()
{
	if (!HasAuthority())
	{
		return;
	}

	Kills = 0;
	Deaths = 0;
	SetScore(0.0f);
	NotifyStatsChanged();
	ForceNetUpdate();
}

void AMultiplayerArenaPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	if (AMultiplayerArenaPlayerState* TargetPlayerState = Cast<AMultiplayerArenaPlayerState>(PlayerState))
	{
		TargetPlayerState->Kills = Kills;
		TargetPlayerState->Deaths = Deaths;
	}
}

void AMultiplayerArenaPlayerState::OverrideWith(APlayerState* PlayerState)
{
	Super::OverrideWith(PlayerState);

	if (const AMultiplayerArenaPlayerState* SourcePlayerState = Cast<AMultiplayerArenaPlayerState>(PlayerState))
	{
		Kills = SourcePlayerState->Kills;
		Deaths = SourcePlayerState->Deaths;
	}
}

void AMultiplayerArenaPlayerState::OnRep_Stats()
{
	NotifyStatsChanged();
}

void AMultiplayerArenaPlayerState::NotifyStatsChanged()
{
	OnPlayerStatsChanged.Broadcast();
}

void AMultiplayerArenaPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMultiplayerArenaPlayerState, Kills);
	DOREPLIFETIME(AMultiplayerArenaPlayerState, Deaths);
}

