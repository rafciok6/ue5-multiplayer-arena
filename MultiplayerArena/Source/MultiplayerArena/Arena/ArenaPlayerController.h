// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MultiplayerArena/MultiplayerArenaPlayerController.h"
#include "ArenaPlayerController.generated.h"

/**
 * 
*/
class UUserWidget;

UCLASS()
class MULTIPLAYERARENA_API AArenaPlayerController : public AMultiplayerArenaPlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void PostSeamlessTravel() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditDefaultsOnly, Category = "Arena|UI")
	TSubclassOf<UUserWidget> ArenaHUDWidgetClass;

	virtual void OnRep_PlayerState() override;

private:
	void ShowArenaHUD();
	void RemoveArenaHUD();

	UPROPERTY()
	TObjectPtr<UUserWidget> ArenaHUDWidget;
};
