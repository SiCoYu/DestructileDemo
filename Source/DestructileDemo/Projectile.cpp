// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/ArrowComponent.h"
#include "Net/UnrealNetwork.h"
#include "GlassPhysicalMaterial.h"

// Sets default values
AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	SetReplicates(true);
	SetReplicateMovement(true);

	Collision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Collision"));
	Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Collision->SetGenerateOverlapEvents(true);
	Collision->SetNotifyRigidBodyCollision(true);
	Collision->SetCollisionResponseToAllChannels(ECR_Block);
	Collision->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	Collision->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	Collision->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);
	Collision->SetCollisionObjectType(ECC_GameTraceChannel1);
	Collision->SetUseCCD(true);
	Collision->bReturnMaterialOnMove = true;
	Collision->OnComponentHit.AddDynamic(this, &AProjectile::HandleHit);
	Collision->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::HandleBeginOverlap);
	Collision->SetCapsuleSize(0.3f, 1.0f);
	RootComponent = Collision;
	Collision->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetGenerateOverlapEvents(false);
	Mesh->SetNotifyRigidBodyCollision(false);
	Mesh->SetCollisionObjectType(ECC_GameTraceChannel1);
	Mesh->SetupAttachment(RootComponent);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement Component"));
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority())
	{
		OnDestroyed.AddDynamic(this, &AProjectile::HandleOnDestroyed);
		TimeSecondsWhenSpawned = GetWorld()->GetTimeSeconds();
		SetLifeSpan(ProjectileValues.LifeTime);
	}
}

FVector AProjectile::GetImpulse() const
{
	return GetActorForwardVector() * (GetVelocity().Size());
}

void AProjectile::HandleHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (HasAuthority() == false)
	{
		return;
	}

	if (OtherActor && OtherActor->GetInstigator())
	{
		UGameplayStatics::ApplyPointDamage(OtherActor, ProjectileValues.BaseDamage, GetImpulse(), Hit, GetOwner()->GetInstigatorController(), this, nullptr);
	}
	else
	{
		ApplyDamage_Multicast(OtherActor, ProjectileValues.BaseDamage, Hit);
	}
	Destroy();
}

void AProjectile::HandleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UGlassPhysicalMaterial* material = Cast<UGlassPhysicalMaterial>(SweepResult.PhysMaterial.Get());
	const bool bIsGlass = material && material->IsDestructableGlass();
	if (!HasAuthority() || !bIsGlass)
	{
		return;
	}

	ApplyDamage_Multicast(OtherActor, ProjectileValues.BaseDamage, SweepResult);
}

void AProjectile::HandleOnDestroyed(AActor* DestroyedActor)
{
	GetWorld()->GetTimerManager().ClearAllTimersForObject(DestroyedActor);
}

void AProjectile::ApplyDamage_Multicast_Implementation(AActor* OtherActor, float Damage, const FHitResult& SweepResult)
{
	UGameplayStatics::ApplyPointDamage(OtherActor, Damage, GetImpulse(), SweepResult, UGameplayStatics::GetPlayerController(this, 0), this, nullptr);
}
