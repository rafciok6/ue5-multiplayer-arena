// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HealthPickup.generated.h"

class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class MULTIPLAYERARENA_API AHealthPickup : public AActor
{
	GENERATED_BODY()

public:
	AHealthPickup();

	UFUNCTION(BlueprintPure, Category = "Pickup")
	bool IsActive() const
	{
		return bIsActive;
	}

protected:
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> CollisionSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> PickupMesh;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category = "Pickup",meta = (ClampMin = "1.0"))
	float HealAmount = 35.0f;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category = "Pickup",meta = (ClampMin = "0.1"))
	float RespawnTime = 8.0f;

	UFUNCTION(BlueprintImplementableEvent,Category = "Pickup",meta = (DisplayName = "On Pickup State Changed"))
	void BP_OnPickupStateChanged(bool bActive);

private:
	UFUNCTION()
	void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,UPrimitiveComponent* OtherComponent,int32 OtherBodyIndex,bool bFromSweep,const FHitResult& SweepResult);

	UFUNCTION()
	void OnRep_IsActive();

	void SetPickupActive(bool bNewActive);
	void UpdatePickupState();
	void RespawnPickup();

	UPROPERTY(ReplicatedUsing = OnRep_IsActive)
	bool bIsActive = true;

	FTimerHandle RespawnTimerHandle;
};