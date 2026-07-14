// Fill out your copyright notice in the Description page of Project Settings.


#include "ArenaPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerState.h"

void AArenaPlayerController::BeginPlay()
{
	Super::BeginPlay();

	ConfigureGameplayInput();
	ShowArenaHUD();
}

void AArenaPlayerController::PostSeamlessTravel()
{
	Super::PostSeamlessTravel();

	ConfigureGameplayInput();
	ShowArenaHUD();
}

void AArenaPlayerController::EndPlay(
	const EEndPlayReason::Type EndPlayReason)
{
	RemoveArenaHUD();

	Super::EndPlay(EndPlayReason);
}

void AArenaPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	ShowArenaHUD();
}

void AArenaPlayerController::ShowArenaHUD()
{
	if (!IsLocalController() || !IsValid(PlayerState) || !ArenaHUDWidgetClass || IsValid(ArenaHUDWidget))
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

void AArenaPlayerController::ConfigureGameplayInput()
{
	if (!IsLocalController())
	{
		return;
	}

	SetInputMode(FInputModeGameOnly());
	bShowMouseCursor = false;

	ResetIgnoreMoveInput();
	ResetIgnoreLookInput();
}