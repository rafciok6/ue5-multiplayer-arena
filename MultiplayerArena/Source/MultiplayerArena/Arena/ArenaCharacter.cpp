// Fill out your copyright notice in the Description page of Project Settings.


#include "ArenaCharacter.h"
#include "Net/UnrealNetwork.h"
#include "ArenaGameState.h"
#include "EnhancedInputComponent.h"
#include "Engine/World.h"
#include "GameFramework/DamageType.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "ArenaGameMode.h"

AArenaCharacter::AArenaCharacter()
{
	bReplicates = true;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility,ECR_Block);
}

float AArenaCharacter::Heal(float HealAmount)
{
	if (!HasAuthority() || bIsDead || HealAmount <= 0.0f ||	CurrentHealth >= MaxHealth)
	{
		return 0.0f;
	}

	const float PreviousHealth = CurrentHealth;

	CurrentHealth = FMath::Clamp(CurrentHealth + HealAmount,0.0f, MaxHealth);

	const float AppliedHealing =
		CurrentHealth - PreviousHealth;

	if (AppliedHealing > 0.0f)
	{
		NotifyHealthChanged();
		ForceNetUpdate();
	}

	return AppliedHealing;
}

void AArenaCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		CurrentHealth = MaxHealth;
		NotifyHealthChanged();
		ForceNetUpdate();
	}
}

void AArenaCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent =Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (FireAction)
		{
			EnhancedInputComponent->BindAction(FireAction,ETriggerEvent::Started,this, &AArenaCharacter::Fire);
		}
	}
}

float AArenaCharacter::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (!HasAuthority() || bIsDead || DamageAmount <= 0.0f)
	{
		return 0.0f;
	}

	const float AppliedDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (AppliedDamage <= 0.0f)
	{
		return 0.0f;
	}

	CurrentHealth = FMath::Clamp(CurrentHealth - AppliedDamage,0.0f, MaxHealth);

	NotifyHealthChanged();
	ForceNetUpdate();

	if (FMath::IsNearlyZero(CurrentHealth))
	{
		bIsDead = true;
		HandleDeath(EventInstigator);
	}

	return AppliedDamage;
}

void AArenaCharacter::OnRep_Health()
{
	NotifyHealthChanged();
}

void AArenaCharacter::NotifyHealthChanged()
{
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}

void AArenaCharacter::HandleDeath(AController* KillerController)
{
	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return;
	}

	AArenaGameMode* ArenaGameMode = World->GetAuthGameMode<AArenaGameMode>();

	if (!IsValid(ArenaGameMode))
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("ArenaGameMode is not available"));
		return;
	}

	ArenaGameMode->HandlePlayerDeath(this, KillerController);
}

void AArenaCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AArenaCharacter, CurrentHealth);
}

void AArenaCharacter::Fire()
{
	if (!IsLocallyControlled())
	{
		return;
	}

	ServerFire();
}

void AArenaCharacter::ServerFire_Implementation()
{
	UWorld* World = GetWorld();

	if (!IsValid(World) || !IsValid(Controller) || bIsDead)
	{
		return;
	}

	if (const AArenaGameState* GameState = World->GetGameState<AArenaGameState>())
	{
		if (GameState->IsMatchFinished())
		{
			return;
		}
	}

	const float CurrentTime = World->GetTimeSeconds();
	if (CurrentTime - LastFireTime < FireInterval)
	{
		return;
	}

	LastFireTime = CurrentTime;

	FVector ViewLocation;
	FRotator ViewRotation;
	Controller->GetPlayerViewPoint(ViewLocation, ViewRotation);

	const FVector TraceDirection = ViewRotation.Vector();
	const FVector TraceEnd = ViewLocation + TraceDirection * FireRange;

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(ArenaFireTrace),false,this);

	QueryParams.AddIgnoredActor(this);

	FHitResult HitResult;
	const bool bHit = World->LineTraceSingleByChannel(HitResult,	ViewLocation,	TraceEnd,ECC_Visibility,	QueryParams);
	const FVector EffectEnd = bHit ? HitResult.ImpactPoint : TraceEnd;

	MulticastPlayFireEffects(ViewLocation, EffectEnd);
	
	AArenaCharacter* HitCharacter = Cast<AArenaCharacter>(HitResult.GetActor());

	if (!bHit || !IsValid(HitCharacter))
	{
		return;
	}
	
	const FString HitCharacterName = HitCharacter->GetName();
	const float AppliedDamage = UGameplayStatics::ApplyPointDamage(HitCharacter, FireDamage, TraceDirection,HitResult,Controller,this, UDamageType::StaticClass());

	if (AppliedDamage > 0.0f)
	{
		UE_LOG(
			LogTemp,
			Log,
			TEXT("%s hit %s for %.0f damage"),
			*GetName(),
			*HitCharacterName,
			AppliedDamage);
	}
}

void AArenaCharacter::MulticastPlayFireEffects_Implementation(FVector_NetQuantize TraceStart, FVector_NetQuantize TraceEnd)
{
	PlayFireEffects(TraceStart, TraceEnd);
}