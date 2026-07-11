// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyPlayerController.h"
#include "LobbyGameMode.h"
#include "Engine/World.h"

void ALobbyPlayerController::ServerRequestStartMatch_Implementation()
{
	UWorld* World = GetWorld();

	if (World == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot request match start: world is invalid"));
		return;
	}

	ALobbyGameMode* LobbyGameMode =	World->GetAuthGameMode<ALobbyGameMode>();

	if (LobbyGameMode == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot request match start: invalid lobby game mode"));
		return;
	}

	LobbyGameMode->TryStartMatch(this);
}