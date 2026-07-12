// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LobbyPlayerController.generated.h"

/**
 * 
 */
class UUserWidget;

UCLASS()
class MULTIPLAYERARENA_API ALobbyPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "Lobby")
	void ServerRequestStartMatch();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void PostSeamlessTravel() override;

	UPROPERTY(EditDefaultsOnly, Category = "Lobby|UI")
	TSubclassOf<UUserWidget> MainMenuWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Lobby|UI")
	TSubclassOf<UUserWidget> LobbyWidgetClass;

	UFUNCTION(BlueprintPure, Category = "Lobby")
	bool IsLobbyHost() const;

private:
	void ShowInitialWidget();
	void ShowWidget(TSubclassOf<UUserWidget> WidgetClass);
	void RemoveActiveWidget();

	UPROPERTY(Transient)
	TObjectPtr<UUserWidget> ActiveWidget;
};
