// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "LobbyGameState.h"
#include "Engine/World.h"

void ALobbyPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALobbyPlayerState, bIsReady);
}

void ALobbyPlayerState::ServerSetReady_Implementation(bool bNewReady)
{
	if (bIsReady == bNewReady)
	{
		return;
	}

	bIsReady = bNewReady;

	//it will run on clients but not on server so called it from here.
	OnRep_IsReady();

	ForceNetUpdate();
}

void ALobbyPlayerState::OnRep_IsReady()
{
	UE_LOG(
		LogTemp,
		Log,
		TEXT("Player %s ready state changed to: %s"),
		*GetPlayerName(),
		bIsReady ? TEXT("Ready") : TEXT("Not Ready"));

	UWorld* World = GetWorld();

	if (World == nullptr)
	{
		return;
	}

	ALobbyGameState* LobbyGameState = World->GetGameState<ALobbyGameState>();

	if (LobbyGameState != nullptr)
	{
		LobbyGameState->NotifyLobbyPlayersChanged();
	}
}