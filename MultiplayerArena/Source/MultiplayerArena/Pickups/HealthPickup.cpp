// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthPickup.h"
#include "../Arena/ArenaCharacter.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"

AHealthPickup::AHealthPickup()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;
	SetReplicateMovement(false);

	NetUpdateFrequency = 2.0f;
	MinNetUpdateFrequency = 1.0f;

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));

	SetRootComponent(CollisionSphere);

	CollisionSphere->InitSphereRadius(75.0f);
	CollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionSphere->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionSphere->SetCollisionResponseToChannel(ECC_Pawn,ECR_Overlap);

	CollisionSphere->OnComponentBeginOverlap.AddDynamic(this,&AHealthPickup::OnSphereBeginOverlap);

	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupMesh"));

	PickupMesh->SetupAttachment(CollisionSphere);
	PickupMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AHealthPickup::BeginPlay()
{
	Super::BeginPlay();

	UpdatePickupState();
}

void AHealthPickup::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority() || !bIsActive)
	{
		return;
	}

	AArenaCharacter* ArenaCharacter = Cast<AArenaCharacter>(OtherActor);

	if (!IsValid(ArenaCharacter))
	{
		return;
	}

	const float AppliedHealing = ArenaCharacter->Heal(HealAmount);

	if (AppliedHealing <= 0.0f)
	{
		return;
	}

	UE_LOG(LogTemp,	Log,
		TEXT("%s collected health pickup for %.0f health"),
		*ArenaCharacter->GetName(),
		AppliedHealing);

	SetPickupActive(false);
	GetWorldTimerManager().SetTimer(RespawnTimerHandle,this,&AHealthPickup::RespawnPickup,RespawnTime,false);
}

void AHealthPickup::SetPickupActive(bool bNewActive)
{
	if (bIsActive == bNewActive)
	{
		return;
	}

	bIsActive = bNewActive;

	UpdatePickupState();
	ForceNetUpdate();
}

void AHealthPickup::RespawnPickup()
{
	if (!HasAuthority())
	{
		return;
	}

	SetPickupActive(true);
}

void AHealthPickup::OnRep_IsActive()
{
	UpdatePickupState();
}

void AHealthPickup::UpdatePickupState()
{
	if (IsValid(CollisionSphere))
	{
		CollisionSphere->SetCollisionEnabled(bIsActive ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
	}

	if (IsValid(PickupMesh))
	{
		PickupMesh->SetVisibility(bIsActive,true);
	}

	BP_OnPickupStateChanged(bIsActive);
}

void AHealthPickup::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AHealthPickup, bIsActive);
}