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

private:
	UFUNCTION()
	void OnRep_IsReady();

	UPROPERTY(ReplicatedUsing = OnRep_IsReady)
	bool bIsReady = false;
};

