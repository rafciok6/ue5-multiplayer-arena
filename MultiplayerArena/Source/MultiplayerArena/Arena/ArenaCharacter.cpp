#include "ArenaCharacter.h"

#include "ArenaGameMode.h"
#include "ArenaGameState.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "Engine/World.h"
#include "GameFramework/DamageType.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "NiagaraFunctionLibrary.h"
#include "Sound/SoundBase.h"

AArenaCharacter::AArenaCharacter()
{
	bReplicates = true;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	PistolMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PistolMesh"));
	PistolMesh->SetupAttachment(GetMesh(), TEXT("hand_r"));
	PistolMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PistolMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	PistolMesh->SetGenerateOverlapEvents(false);
}

float AArenaCharacter::Heal(float HealAmount)
{
	if (!HasAuthority() || bIsDead || HealAmount <= 0.0f || CurrentHealth >= MaxHealth)
	{
		return 0.0f;
	}

	const float PreviousHealth = CurrentHealth;

	CurrentHealth = FMath::Clamp(CurrentHealth + HealAmount, 0.0f, MaxHealth);

	const float AppliedHealing = CurrentHealth - PreviousHealth;

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

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	if (IsValid(EnhancedInputComponent) && IsValid(FireAction))
	{
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &AArenaCharacter::Fire);
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

	CurrentHealth = FMath::Clamp(CurrentHealth - AppliedDamage,  0.0f, MaxHealth);

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
		UE_LOG(LogTemp, Error,
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

	const AArenaGameState* GameState = World->GetGameState<AArenaGameState>();

	if (IsValid(GameState) && GameState->IsMatchFinished())
	{
		return;
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

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(ArenaFireTrace), false, this);
	QueryParams.AddIgnoredActor(this);

	const FVector ViewDirection = ViewRotation.Vector();
	const FVector CameraTraceEnd = ViewLocation + ViewDirection * FireRange;

	FHitResult CameraHitResult;
	const bool bCameraHit = World->LineTraceSingleByChannel(CameraHitResult, ViewLocation, CameraTraceEnd, ECC_Visibility, QueryParams);

	const FVector AimPoint = bCameraHit ? CameraHitResult.ImpactPoint : CameraTraceEnd;
	const FTransform MuzzleTransform = GetMuzzleTransform();
	const FVector MuzzleLocation = MuzzleTransform.GetLocation();

	FVector ShotDirection = (AimPoint - MuzzleLocation).GetSafeNormal();

	if (ShotDirection.IsNearlyZero())
	{
		ShotDirection = ViewDirection;
	}

	const FVector ShotTraceEnd = MuzzleLocation + ShotDirection * FireRange;

	FHitResult HitResult;
	const bool bHit = World->LineTraceSingleByChannel(HitResult, MuzzleLocation, ShotTraceEnd, ECC_Visibility, QueryParams);
	const FVector EffectEnd = bHit ? HitResult.ImpactPoint : ShotTraceEnd;
	const FVector ImpactNormal = bHit ? HitResult.ImpactNormal : -ShotDirection;
	MulticastPlayFireEffects(MuzzleLocation, EffectEnd, ImpactNormal, bHit);

	AArenaCharacter* HitCharacter = Cast<AArenaCharacter>(HitResult.GetActor());

	if (!bHit || !IsValid(HitCharacter))
	{
		return;
	}

	const FString HitCharacterName = HitCharacter->GetName();
	const float AppliedDamage = UGameplayStatics::ApplyPointDamage(HitCharacter, FireDamage, ShotDirection, HitResult, Controller, this, UDamageType::StaticClass());

	if (AppliedDamage > 0.0f)
	{
		UE_LOG(LogTemp, Log,
			TEXT("%s hit %s for %.0f damage"),
			*GetName(),
			*HitCharacterName,
			AppliedDamage);
	}
}

FTransform AArenaCharacter::GetMuzzleTransform() const
{
	if (!IsValid(PistolMesh))
	{
		return GetActorTransform();
	}

	if (PistolMesh->DoesSocketExist(MuzzleSocketName))
	{
		return PistolMesh->GetSocketTransform(MuzzleSocketName, RTS_World);
	}

	return PistolMesh->GetComponentTransform();
}

void AArenaCharacter::PlayFireEffects(const FVector& MuzzleLocation, const FVector& EffectEnd, const FVector& ImpactNormal, bool bHit) const
{
	const FRotator ShotRotation = (EffectEnd - MuzzleLocation).Rotation();

	if (IsValid(MuzzleEffect))
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, MuzzleEffect, MuzzleLocation, ShotRotation, FVector(MuzzleEffectScale));
	}

	if (IsValid(FireSound))
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, MuzzleLocation); 
	}

	if (bHit && IsValid(ImpactEffect))
	{
		const FRotator ImpactRotation = FRotationMatrix::MakeFromZ(ImpactNormal).Rotator();

		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, EffectEnd, ImpactRotation, FVector(ImpactEffectScale));
	}
}

void AArenaCharacter::MulticastPlayFireEffects_Implementation(FVector_NetQuantize MuzzleLocation, FVector_NetQuantize EffectEnd, FVector_NetQuantizeNormal ImpactNormal, bool bHit)
{
	PlayFireEffects(MuzzleLocation, EffectEnd, ImpactNormal, bHit);
}
