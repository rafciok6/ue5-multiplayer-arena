// Fill out your copyright notice in the Description page of Project Settings.


#include "ArenaPlayerController.h"
#include "Blueprint/UserWidget.h"

void AArenaPlayerController::BeginPlay()
{
	Super::BeginPlay();

	ShowArenaHUD();
}

void AArenaPlayerController::PostSeamlessTravel()
{
	Super::PostSeamlessTravel();

	ShowArenaHUD();
}

void AArenaPlayerController::EndPlay(
	const EEndPlayReason::Type EndPlayReason)
{
	RemoveArenaHUD();

	Super::EndPlay(EndPlayReason);
}

void AArenaPlayerController::ShowArenaHUD()
{
	if (!IsLocalController() ||	!ArenaHUDWidgetClass ||	IsValid(ArenaHUDWidget))
	{
		return;
	}

	ArenaHUDWidget = CreateWidget<UUserWidget>(this, ArenaHUDWidgetClass);

	if (!IsValid(ArenaHUDWidget))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create arena HUD"));
		return;
	}

	ArenaHUDWidget->AddToPlayerScreen();

	UE_LOG(LogTemp, Log, TEXT("Arena HUD added to player screen"));
}

void AArenaPlayerController::RemoveArenaHUD()
{
	if (!IsValid(ArenaHUDWidget))
	{
		return;
	}

	ArenaHUDWidget->RemoveFromParent();
	ArenaHUDWidget = nullptr;
}