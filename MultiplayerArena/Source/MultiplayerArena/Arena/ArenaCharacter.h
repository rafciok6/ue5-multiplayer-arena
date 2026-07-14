// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MultiplayerArena/MultiplayerArenaCharacter.h"
#include "ArenaCharacter.generated.h"

class UInputAction;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChanged, float, CurrentHealth, float, MaxHealth);

UCLASS()
class MULTIPLAYERARENA_API AArenaCharacter : public AMultiplayerArenaCharacter
{
	GENERATED_BODY()

public:
	AArenaCharacter();

	virtual float TakeDamage(float DamageAmount,const FDamageEvent& DamageEvent,AController* EventInstigator,AActor* DamageCauser) override;

	UFUNCTION(BlueprintPure, Category = "Combat")
	float GetCurrentHealth() const
	{
		return CurrentHealth;
	}

	UFUNCTION(BlueprintPure, Category = "Combat")
	float GetMaxHealth() const
	{
		return MaxHealth;
	}

	UFUNCTION(BlueprintPure, Category = "Combat")
	float GetHealthPercent() const
	{
		return MaxHealth > 0.0f ? CurrentHealth / MaxHealth : 0.0f;
	}

	UFUNCTION(BlueprintCallable, Category = "Combat")
	float Heal(float HealAmount);

	UPROPERTY(BlueprintAssignable, Category = "Combat")
	FOnHealthChanged OnHealthChanged;

protected:
	virtual void BeginPlay() override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> FireAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Weapon", meta = (ClampMin = "0.0"))
	float FireDamage = 34.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Weapon", meta = (ClampMin = "100.0"))
	float FireRange = 5000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Weapon", meta = (ClampMin = "0.1"))
	float FireInterval = 0.25f;

	UFUNCTION(BlueprintImplementableEvent, Category = "Combat|Weapon")
	void PlayFireEffects(FVector TraceStart, FVector TraceEnd);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat", meta = (ClampMin = "1.0"))
	float MaxHealth = 100.0f;

	UPROPERTY(ReplicatedUsing = OnRep_Health, BlueprintReadOnly, Category = "Combat")
	float CurrentHealth = 0.0f;

private:
	UFUNCTION()
	void OnRep_Health();

	void NotifyHealthChanged();
	void HandleDeath(AController* KillerController);

	void Fire();

	UFUNCTION(Server, Reliable)
	void ServerFire();

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastPlayFireEffects(FVector_NetQuantize TraceStart, FVector_NetQuantize TraceEnd);

	bool bIsDead = false;
	float LastFireTime = -1000.0f;
};
