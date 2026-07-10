// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "MultiplayerSessionSubsystem.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class MULTIPLAYERARENA_API UMultiplayerSessionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Category = "Multiplayer|Sessions")
	void CreateSession(int32 NumPublicConnections = 4);
	
private:
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);

	FDelegateHandle CreateSessionCompleteDelegateHandle;
	IOnlineSessionPtr SessionInterface;
};
