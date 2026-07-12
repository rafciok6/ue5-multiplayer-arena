// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "LobbyPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERARENA_API ALobbyPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintPure, Category = "Lobby")
	bool IsReady() const
	{
		return bIsReady;
	}

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "Lobby")
	void ServerSetReady(bool bNewReady);

	virtual void SetPlayerName(const FString& Name) override;
	virtual void OnRep_PlayerName() override;

private:
	UFUNCTION()
	void OnRep_IsReady();
	void NotifyLobbyPlayersChanged() const;

	UPROPERTY(ReplicatedUsing = OnRep_IsReady)
	bool bIsReady = false;
};

