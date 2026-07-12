// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyPlayerController.h"
#include "LobbyGameMode.h"
#include "Engine/World.h"
#include "Blueprint/UserWidget.h"

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

void ALobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	ShowInitialWidget();
}

void ALobbyPlayerController::PostSeamlessTravel()
{
	Super::PostSeamlessTravel();

	ShowInitialWidget();
}

void ALobbyPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	RemoveActiveWidget();

	if (IsLocalController())
	{
		bShowMouseCursor = false;
		SetInputMode(FInputModeGameOnly());
	}

	Super::EndPlay(EndPlayReason);
}

void ALobbyPlayerController::ShowInitialWidget()
{
	if (!IsLocalController())
	{
		return;
	}

	const TSubclassOf<UUserWidget> WidgetClass = GetNetMode() == NM_Standalone
			? MainMenuWidgetClass
			: LobbyWidgetClass;

	ShowWidget(WidgetClass);
}

void ALobbyPlayerController::ShowWidget(TSubclassOf<UUserWidget> WidgetClass)
{
	RemoveActiveWidget();

	if (WidgetClass == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot show UI: widget class is not configured"));
		return;
	}

	ActiveWidget = CreateWidget<UUserWidget>(this, WidgetClass);

	if (ActiveWidget == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create lobby UI widget"));
		return;
	}

	ActiveWidget->AddToViewport();

	FInputModeUIOnly InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

	SetInputMode(InputMode);
	bShowMouseCursor = true;
}

void ALobbyPlayerController::RemoveActiveWidget()
{
	if (ActiveWidget != nullptr)
	{
		ActiveWidget->RemoveFromParent();
		ActiveWidget = nullptr;
	}
}
