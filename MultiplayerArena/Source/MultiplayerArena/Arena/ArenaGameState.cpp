// Fill out your copyright notice in the Description page of Project Settings.

#include "ArenaGameState.h"
#include "Net/UnrealNetwork.h"

void AArenaGameState::SetRemainingMatchTime(int32 NewRemainingTime)
{
	if (!HasAuthority())
	{
		return;
	}

	const int32 ClampedTime = FMath::Max(0, NewRemainingTime);

	if (RemainingMatchTime == ClampedTime)
	{
		return;
	}

	RemainingMatchTime = ClampedTime;
	NotifyMatchStateChanged();
	ForceNetUpdate();
}

void AArenaGameState::SetMatchFinished(bool bNewMatchFinished)
{
	if (!HasAuthority() || bMatchFinished == bNewMatchFinished)
	{
		return;
	}

	bMatchFinished = bNewMatchFinished;
	NotifyMatchStateChanged();
	ForceNetUpdate();
}

void AArenaGameState::OnRep_RemainingMatchTime()
{
	NotifyMatchStateChanged();
}

void AArenaGameState::OnRep_MatchFinished()
{
	NotifyMatchStateChanged();
}

void AArenaGameState::NotifyMatchStateChanged()
{
	OnMatchStateChanged.Broadcast();
}

void AArenaGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AArenaGameState, RemainingMatchTime);
	DOREPLIFETIME(AArenaGameState, bMatchFinished);
}