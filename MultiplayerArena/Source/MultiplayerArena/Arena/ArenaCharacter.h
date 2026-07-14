#pragma once

#include "CoreMinimal.h"
#include "MultiplayerArena/MultiplayerArenaCharacter.h"
#include "ArenaCharacter.generated.h"

class UInputAction;
class UNiagaraSystem;
class USoundBase;
class UStaticMeshComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChanged, float, CurrentHealth, float, MaxHealth);

UCLASS()
class MULTIPLAYERARENA_API AArenaCharacter : public AMultiplayerArenaCharacter
{
	GENERATED_BODY()

public:
	AArenaCharacter();

	virtual float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

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
		return MaxHealth > 0.0f
			? CurrentHealth / MaxHealth
			: 0.0f;
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|Weapon")
	TObjectPtr<UStaticMeshComponent> PistolMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Weapon")
	FName MuzzleSocketName = TEXT("Muzzle");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Weapon", meta = (ClampMin = "0.0"))
	float FireDamage = 34.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Weapon", meta = (ClampMin = "100.0"))
	float FireRange = 5000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Weapon", meta = (ClampMin = "0.1"))
	float FireInterval = 0.25f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Weapon|Effects")
	TObjectPtr<UNiagaraSystem> MuzzleEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Weapon|Effects")
	TObjectPtr<UNiagaraSystem> ImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Weapon|Effects")
	TObjectPtr<USoundBase> FireSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Weapon|Effects", meta = (ClampMin = "0.01"))
	float MuzzleEffectScale = 0.35f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Weapon|Effects", meta = (ClampMin = "0.01"))
	float ImpactEffectScale = 0.6f;

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
	FTransform GetMuzzleTransform() const;

	void PlayFireEffects(const FVector& MuzzleLocation, const FVector& EffectEnd, const FVector& ImpactNormal, bool bHit) const;

	UFUNCTION(Server, Reliable)
	void ServerFire();

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastPlayFireEffects(FVector_NetQuantize MuzzleLocation, FVector_NetQuantize EffectEnd, FVector_NetQuantizeNormal ImpactNormal, bool bHit);

	bool bIsDead = false;
	float LastFireTime = -1000.0f;
};
